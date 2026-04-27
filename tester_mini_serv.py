#!/usr/bin/env python3
"""
tester_mini_serv.py — Test suite for mini_serv

Usage:
  python3 tester_mini_serv.py <executable> [options]

Options:
  --valgrind        Run the server under Valgrind to detect memory leaks.
  --port PORT       Use a fixed port instead of a random one.

Examples:
  python3 tester_mini_serv.py ./mini_serv
  python3 tester_mini_serv.py ./mini_serv --valgrind
  python3 tester_mini_serv.py ./mini_serv --port 8080
  python3 tester_mini_serv.py ./mini_serv --valgrind --port 8080

Manual debugging (fixed port):
  Traffic inspection:  sudo tcpdump -i lo -A -s 0 -nn port 8080
  Hex + ASCII:         sudo tcpdump -i lo -X -s 0 -nn port 8080
"""

import argparse
import os
import re
import signal
import shutil
import socket
import struct
import subprocess
import sys
import threading
import time
import random
import atexit

_active_procs = []

def _kill_all_procs():
    for p in list(_active_procs):
        try:
            if p.poll() is None:
                p.kill()
                p.wait(timeout=2.0)
        except Exception:
            pass

atexit.register(_kill_all_procs)

def _sigint_handler(sig, frame):
    _kill_all_procs()
    sys.exit(1)

signal.signal(signal.SIGINT, _sigint_handler)

def _free_port(port):
    """Kill any process holding the given TCP port."""
    try:
        result = subprocess.run(
            ["fuser", f"{port}/tcp"],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL
        )
        pids = result.stdout.decode().split()
        for pid in pids:
            try:
                os.kill(int(pid), signal.SIGKILL)
            except Exception:
                pass
        if pids:
            time.sleep(0.3)
    except Exception:
        pass


HOST            = "127.0.0.1"
TIMEOUT         = 5.0
STARTUP_TIMEOUT = 2.5

ARRIVED_RE = re.compile(rb"server: client \d+ just arrived\n")
LEFT_RE    = re.compile(rb"server: client \d+ just left\n")

FORBIDDEN_FUNCTIONS = [
    "printf", "fcntl", "read", "open", "fork",
    "execve", "execv", "execvp",
    "poll", "epoll_create", "epoll_wait", "epoll_ctl",
    "kqueue", "kevent",
    "dup2", "dup",
    "pipe",
]


# ──────────────────────────────────────────────────────────────────────────────
# Helpers
# ──────────────────────────────────────────────────────────────────────────────

def fail(msg):
    print(f"\n[FAIL] {msg}", flush=True)
    sys.exit(1)


def get_free_port():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, 0))
    port = s.getsockname()[1]
    s.close()
    return port


def run_cmd_capture(cmd, timeout=2.0):
    proc = subprocess.run(
        cmd,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=timeout,
    )
    return proc.returncode, proc.stdout, proc.stderr


# ──────────────────────────────────────────────────────────────────────────────
# Static source analysis
# ──────────────────────────────────────────────────────────────────────────────

def check_source_rules(src_path="mini_serv.c"):
    if not os.path.exists(src_path):
        print(f"[SKIP] {src_path} not found")
        return

    with open(src_path, "r", encoding="utf-8", errors="replace") as f:
        src = f.read()

    if "#define" in src:
        fail(f"#define is not allowed and was found in {src_path}")

    for fn in FORBIDDEN_FUNCTIONS:
        if re.search(r'\b' + re.escape(fn) + r'\s*\(', src):
            fail(f"Forbidden function '{fn}' detected in {src_path}")

    if not re.search(r'\bselect\s*\(', src):
        fail(f"select() not found in {src_path} — required for I/O multiplexing")

    if not ("fd_set" in src or "FD_SET" in src):
        fail(f"fd_set/FD_SET not found in {src_path}")

    if "FD_ZERO" not in src:
        fail(f"FD_ZERO not found in {src_path}")

    if not re.search(r'\bmalloc\s*\(', src):
        fail(f"malloc() not found in {src_path} — dynamic allocation required")

    if not re.search(r'\bfree\s*\(', src):
        fail(f"free() not found in {src_path} — all malloc'd memory must be freed")

    if re.search(r'\bsend\s*\(', src):
        if not re.search(r'(send\s*\([^;]+\))\s*[<>!=]|[<>!=]\s*(send\s*\()|=\s*send\s*\(', src):
            print("[WARN] send() return value may not be checked")

    if re.search(r'\brecv\s*\(', src):
        if not re.search(r'(recv\s*\([^;]+\))\s*[<>!=]|[<>!=]\s*(recv\s*\()|=\s*recv\s*\(', src):
            print("[WARN] recv() return value may not be checked")

    if not re.search(
        r'(malloc\s*\([^)]+\))\s*==\s*NULL|NULL\s*==\s*(malloc\s*\()|'
        r'!\s*malloc\s*\(|if\s*\(\s*!\s*\w', src
    ):
        print("[WARN] malloc() return value may not be NULL-checked")

    print("[OK] static source check passed")


# ──────────────────────────────────────────────────────────────────────────────
# Server and Client infrastructure
# ──────────────────────────────────────────────────────────────────────────────

class Server:
    def __init__(self, exe, port, use_valgrind=False):
        self.exe = exe
        self.port = port
        self.use_valgrind = use_valgrind and shutil.which("valgrind") is not None

        cmd = [exe, str(port)]
        if self.use_valgrind:
            cmd = [
                "valgrind",
                "--leak-check=full",
                "--show-leak-kinds=all",
                "--errors-for-leak-kinds=all",
                "--error-exitcode=42",
            ] + cmd

        self.proc = subprocess.Popen(
            cmd,
            stdin=subprocess.DEVNULL,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
        )
        _active_procs.append(self.proc)

    def stop(self):
        try:
            _active_procs.remove(self.proc)
        except ValueError:
            pass
        if self.proc.poll() is None:
            try:
                sig = signal.SIGINT if self.use_valgrind else signal.SIGTERM
                os.kill(self.proc.pid, sig)
            except Exception:
                pass
            try:
                self.proc.wait(timeout=3.0)
            except subprocess.TimeoutExpired:
                try:
                    self.proc.kill()
                    self.proc.wait(timeout=1.0)
                except Exception:
                    pass

        stderr = b""
        try:
            stderr = self.proc.stderr.read() or b""
        except Exception:
            pass
        return self.proc.returncode, stderr

    def is_alive(self):
        return self.proc.poll() is None


class Client:
    def __init__(self, port, label, lazy=False):
        self.label   = label
        self.lazy    = lazy
        self._buf    = b""
        self._lock   = threading.Lock()
        self._closed = False
        self.sock    = None

        deadline = time.time() + STARTUP_TIMEOUT
        last_err = None

        while time.time() < deadline:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.settimeout(0.25)
                s.connect((HOST, port))
                s.settimeout(None)
                self.sock = s
                break
            except OSError as e:
                last_err = e
                try:
                    s.close()
                except Exception:
                    pass
                time.sleep(0.02)

        if self.sock is None:
            raise RuntimeError(f"Client {label} could not connect: {last_err}")

        self.sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self._th = threading.Thread(target=self._reader, daemon=True)
        self._th.start()

    def _reader(self):
        if self.lazy:
            return
        try:
            while True:
                data = self.sock.recv(4096)
                if not data:
                    break
                with self._lock:
                    self._buf += data
        except Exception:
            pass

    def send(self, msg: str):
        self.sock.sendall(msg.encode())

    def clear(self):
        with self._lock:
            self._buf = b""

    def expect_re(self, pattern: bytes, where=""):
        deadline = time.time() + TIMEOUT
        while time.time() < deadline:
            with self._lock:
                m = re.search(pattern, self._buf)
                if m:
                    self._buf = self._buf.replace(m.group(0), b"", 1)
                    return m.group(0)
            time.sleep(0.02)
        fail(f"Client {self.label} did not receive pattern {pattern!r} [{where}]")

    def expect_sub(self, text: bytes, where=""):
        deadline = time.time() + TIMEOUT
        while time.time() < deadline:
            with self._lock:
                if text in self._buf:
                    self._buf = self._buf.replace(text, b"", 1)
                    return
            time.sleep(0.02)
        fail(f"Client {self.label} did not receive {text!r} [{where}]")

    def not_received(self, text: bytes, where=""):
        time.sleep(0.25)
        with self._lock:
            if text in self._buf:
                fail(f"Client {self.label} should NOT have received {text!r} [{where}]")

    def close(self):
        if self._closed:
            return
        self._closed = True
        try:
            self.sock.shutdown(socket.SHUT_RDWR)
        except Exception:
            pass
        try:
            self.sock.close()
        except Exception:
            pass
        try:
            self._th.join(timeout=0.2)
        except Exception:
            pass

    def close_rst(self):
        """Close the connection with RST instead of FIN (SO_LINGER l_linger=0)."""
        if self._closed:
            return
        self._closed = True
        try:
            self.sock.setsockopt(
                socket.SOL_SOCKET,
                socket.SO_LINGER,
                struct.pack("ii", 1, 0),
            )
        except Exception:
            pass
        try:
            self.sock.close()
        except Exception:
            pass


# ──────────────────────────────────────────────────────────────────────────────
# Tests
# ──────────────────────────────────────────────────────────────────────────────

def test_wrong_arg_count(exe):
    print("[1] wrong argument count ...")

    rc, out, err = run_cmd_capture([exe])
    if rc != 1:
        fail(f"no args: expected returncode 1, got {rc}")
    if out not in (b"", None):
        fail(f"no args: unexpected stdout: {out!r}")
    if err != b"Wrong number of arguments\n":
        fail(f"no args: unexpected stderr: {err!r}")

    rc, out, err = run_cmd_capture([exe, "123", "456"])
    if rc != 1:
        fail(f"extra args: expected returncode 1, got {rc}")
    if out not in (b"", None):
        fail(f"extra args: unexpected stdout: {out!r}")
    if err != b"Wrong number of arguments\n":
        fail(f"extra args: unexpected stderr: {err!r}")

    print("    OK")


def test_bind_conflict(exe):
    print("[2] bind error before accepting connections ...")

    guard = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    guard.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    guard.bind((HOST, 0))
    port = guard.getsockname()[1]
    guard.listen(1)

    rc, out, err = run_cmd_capture([exe, str(port)])
    guard.close()

    if rc != 1:
        fail(f"bind conflict: expected returncode 1, got {rc}")
    if out not in (b"", None):
        fail(f"bind conflict: unexpected stdout: {out!r}")
    if err != b"Fatal error\n":
        fail(f"bind conflict: unexpected stderr: {err!r}")

    print("    OK")


def test_localhost_only(exe):
    print("[3] bind restricted to 127.0.0.1 ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        time.sleep(0.15)

        good = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        good.settimeout(0.5)
        try:
            good.connect((HOST, port))
        except Exception as e:
            fail(f"Could not connect to 127.0.0.1:{port}: {e}")
        finally:
            try:
                good.close()
            except Exception:
                pass

        bad = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        bad.settimeout(0.5)
        try:
            if bad.connect_ex(("127.0.0.2", port)) == 0:
                fail(f"Server accepted connection on 127.0.0.2:{port}")
        finally:
            try:
                bad.close()
            except Exception:
                pass
    finally:
        server.stop()

    print("    OK")


def test_arrived(exe):
    print("[4] just arrived ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        clients = []
        for i in range(4):
            c = Client(port, i)
            clients.append(c)
            if i > 0:
                arrival = f"server: client {i} just arrived\n".encode()
                for prev in clients[:-1]:
                    prev.expect_sub(arrival, where=f"arrived client {i}")
                c.not_received(arrival, where=f"sender should not receive own arrived {i}")
        for c in clients:
            c.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_broadcast(exe):
    print("[5] broadcast format 'client X: msg' ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        a = Client(port, 0)
        b = Client(port, 1)
        c = Client(port, 2)

        time.sleep(0.25)
        a.clear(); b.clear(); c.clear()

        a.send("hello world\n")

        pattern = rb"client \d+: hello world\n"
        b.expect_re(pattern, where="broadcast b")
        c.expect_re(pattern, where="broadcast c")
        a.not_received(b"hello world\n", where="sender should not receive own message")

        for cl in (a, b, c):
            cl.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_partial_message(exe):
    print("[6] fragmented message buffered until newline ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        a = Client(port, 0)
        b = Client(port, 1)

        time.sleep(0.2)
        a.clear(); b.clear()

        a.send("partial message ")
        b.not_received(b"client 0: partial message ", where="should not emit without newline")

        a.send("complete\n")
        b.expect_sub(b"client 0: partial message complete\n", where="reassembled message")

        a.close(); b.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_multi_message_packet(exe):
    print("[7] multiple messages in a single recv ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        a = Client(port, 0)
        b = Client(port, 1)

        time.sleep(0.2)
        a.clear(); b.clear()

        a.send("first\nsecond\n")

        b.expect_sub(b"client 0: first\n",  where="first message")
        b.expect_sub(b"client 0: second\n", where="second message")
        a.not_received(b"first\n", where="sender should not receive own message")

        a.close(); b.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_left(exe):
    print("[8] just left ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        a = Client(port, 0)
        b = Client(port, 1)

        time.sleep(0.2)
        a.clear(); b.clear()

        b.close()
        time.sleep(0.3)

        a.expect_re(LEFT_RE, where="just left")

        a.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_id_monotonicity(exe):
    print("[9] monotonically increasing IDs across disconnects ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        c0 = Client(port, 0)
        c1 = Client(port, 1)

        time.sleep(0.2)
        c0.expect_sub(b"server: client 1 just arrived\n", where="arrived 1")

        c2 = Client(port, 2)
        time.sleep(0.2)
        c0.expect_sub(b"server: client 2 just arrived\n", where="arrived 2")
        c1.expect_sub(b"server: client 2 just arrived\n", where="arrived 2")

        c1.close()
        time.sleep(0.3)
        c0.expect_sub(b"server: client 1 just left\n", where="left 1")
        c2.expect_sub(b"server: client 1 just left\n", where="left 1")

        c3 = Client(port, 3)
        time.sleep(0.2)
        c0.expect_sub(b"server: client 3 just arrived\n", where="arrived 3")
        c2.expect_sub(b"server: client 3 just arrived\n", where="arrived 3")

        for c in (c0, c2, c3):
            c.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_lazy_client(exe):
    print("[10] slow reader does not block the server ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        lazy = Client(port, 0, lazy=True)
        a = Client(port, 1)
        b = Client(port, 2)

        time.sleep(0.2)
        a.clear(); b.clear()

        for i in range(2000):
            a.send(f"spam {i}\n")

        time.sleep(0.8)

        a.clear(); b.clear()
        a.send("still alive\n")
        b.expect_sub(b"client 1: still alive\n", where="server alive after lazy client")

        lazy.close(); a.close(); b.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_large_message(exe):
    print("[11] large message (~4096 bytes) ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        a = Client(port, 0)
        b = Client(port, 1)

        time.sleep(0.2)
        a.clear(); b.clear()

        big = "x" * 4095 + "\n"
        a.send(big)

        expected = b"client 0: " + (b"x" * 4095) + b"\n"
        b.expect_sub(expected, where="large message complete")

        a.close(); b.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_stress(exe):
    print("[12] stress test (multiple clients, many rounds) ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        n = 15
        rounds = 100

        clients = [Client(port, i) for i in range(n)]
        time.sleep(0.4)

        for c in clients:
            c.clear()

        for i in range(rounds):
            sender = random.choice(clients)
            sender.send(f"r{i}_from_{sender.label}\n")
            time.sleep(0.005)

        time.sleep(0.4)

        ping_sender = clients[0]
        others = clients[1:]
        for c in others:
            c.clear()

        ping_sender.send("ping_final\n")
        for dest in others:
            dest.expect_sub(b"ping_final\n", where="stress ping_final")

        for c in clients:
            c.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def _spawn_connector(port):
    script = (
        f"import socket, time; "
        f"s = socket.socket(socket.AF_INET, socket.SOCK_STREAM); "
        f"s.connect(('{HOST}', {port})); "
        f"time.sleep(60)"
    )
    return subprocess.Popen(
        [sys.executable, "-c", script],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def test_rst_disconnect(exe):
    print("[13] abrupt disconnection (SIGKILL → kernel RST) ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        a = Client(port, 0)
        b = Client(port, 1)

        rst_proc = _spawn_connector(port)
        time.sleep(0.35)
        a.clear(); b.clear()

        rst_proc.kill()
        rst_proc.wait(timeout=3.0)

        if not server.is_alive():
            fail("server crashed after receiving RST from a client")

        a.expect_re(LEFT_RE, where="just left after RST")

        a.clear(); b.clear()
        a.send("after_rst\n")
        b.expect_sub(b"client 0: after_rst\n", where="communication after RST")

        a.close(); b.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_sigpipe_resistance(exe):
    print("[14] SIGPIPE resistance (send to dead socket after SIGKILL) ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        a = Client(port, 0)
        b = Client(port, 1)

        dead_proc = _spawn_connector(port)
        time.sleep(0.3)

        a.clear(); b.clear()
        for i in range(10):
            a.send(f"pre_kill_{i}\n")
        time.sleep(0.1)

        dead_proc.kill()
        dead_proc.wait(timeout=3.0)
        time.sleep(0.15)

        a.clear(); b.clear()
        a.send("sigpipe_test\n")
        b.expect_sub(b"client 0: sigpipe_test\n", where="server alive after SIGPIPE")

        if not server.is_alive():
            fail(
                "Server crashed on send() to a dead socket (SIGPIPE).\n"
                "  Fix: add signal(SIGPIPE, SIG_IGN) in main(), "
                "or use MSG_NOSIGNAL in every send() call."
            )

        a.close(); b.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_recv_returns_zero(exe):
    print("[15] recv() == 0 (graceful EOF) handled correctly ...")
    port = get_free_port()
    server = Server(exe, port)
    try:
        watcher = Client(port, 0)
        leaver  = Client(port, 1)

        time.sleep(0.2)
        watcher.clear()

        leaver.close()
        time.sleep(0.3)

        watcher.expect_re(LEFT_RE, where="just left on EOF")

        next_id = 2
        for i in range(5):
            new_c = Client(port, next_id)
            time.sleep(0.05)
            watcher.clear()
            new_c.send(f"alive_{i}\n")
            watcher.expect_sub(
                f"client {next_id}: alive_{i}\n".encode(),
                where=f"server alive after EOF cycle {i}",
            )
            new_c.close()
            next_id += 1
            time.sleep(0.05)

        watcher.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_select_not_busyloop(exe):
    print("[16] select() blocks (no busy-loop) ...")

    if not shutil.which("ps"):
        print("    [SKIP] 'ps' not available")
        return

    port = get_free_port()
    server = Server(exe, port)
    try:
        c = Client(port, 0)
        time.sleep(0.3)

        try:
            result = subprocess.run(
                ["ps", "-p", str(server.proc.pid), "-o", "pcpu="],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=2.0,
            )
            cpu_str = result.stdout.decode().strip()
            if cpu_str:
                cpu = float(cpu_str)
                if cpu > 50.0:
                    fail(
                        f"Server CPU usage = {cpu:.1f}% at idle — possible busy-loop.\n"
                        f"  select() must block until activity is detected."
                    )
                print(f"    CPU at idle: {cpu:.1f}%")
            else:
                print("    [SKIP] ps returned no CPU data")
        except Exception as e:
            print(f"    [SKIP] could not measure CPU: {e}")

        c.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")


def test_no_stdout(exe):
    print("[17] no output on stdout during normal operation ...")
    port = get_free_port()

    proc = subprocess.Popen(
        [exe, str(port)],
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
    )
    try:
        time.sleep(0.15)
        a = Client(port, 0)
        b = Client(port, 1)
        time.sleep(0.1)
        a.send("hello\n")
        b.expect_sub(b"client 0: hello\n", where="stdout test")
        a.close(); b.close()
        time.sleep(0.2)
    finally:
        proc.terminate()
        try:
            out, _ = proc.communicate(timeout=2.0)
        except subprocess.TimeoutExpired:
            proc.kill()
            out, _ = proc.communicate()

    if out and out.strip():
        fail(f"Server wrote to stdout: {out!r}")

    print("    OK")


def test_concurrent_connect_disconnect(exe):
    print("[18] concurrent connect/disconnect does not corrupt state ...")
    port = get_free_port()
    server = Server(exe, port)
    errors = []

    def connect_disconnect(i):
        try:
            c = Client(port, 100 + i)
            time.sleep(random.uniform(0.01, 0.1))
            c.send(f"concurrent_{i}\n")
            time.sleep(random.uniform(0.01, 0.05))
            c.close()
        except Exception as e:
            errors.append(str(e))

    try:
        stable_a = Client(port, 0)
        stable_b = Client(port, 1)
        time.sleep(0.2)
        stable_a.clear(); stable_b.clear()

        threads = [threading.Thread(target=connect_disconnect, args=(i,)) for i in range(20)]
        for t in threads:
            t.start()
        for t in threads:
            t.join(timeout=TIMEOUT)

        if errors:
            fail(f"Errors in concurrent connection threads: {errors[:3]}")

        if not server.is_alive():
            fail("Server crashed during concurrent connect/disconnect")

        stable_a.clear(); stable_b.clear()
        stable_a.send("sanity_check\n")
        stable_b.expect_sub(b"client 0: sanity_check\n", where="sanity after concurrency")

        stable_a.close(); stable_b.close()
        time.sleep(0.2)
    finally:
        server.stop()

    print("    OK")




# ──────────────────────────────────────────────────────────────────────────────
# Valgrind
# ──────────────────────────────────────────────────────────────────────────────

def _valgrind_summary_ok(stderr_text):
    cleaned = "\n".join(
        line for line in stderr_text.splitlines()
        if "WARNING: unhandled" not in line
    )
    if "All heap blocks were freed -- no leaks are possible" in cleaned:
        return True
    if "in use at exit: 0 bytes in 0 blocks" in cleaned and "ERROR SUMMARY: 0 errors" in cleaned:
        return True
    if (
        "definitely lost: 0 bytes in 0 blocks" in cleaned
        and "indirectly lost: 0 bytes in 0 blocks" in cleaned
    ):
        return True
    return False


def test_valgrind_smoke(exe):
    if shutil.which("valgrind") is None:
        print("[SKIP] valgrind not installed")
        return

    print("[19] smoke test under valgrind ...")
    port = get_free_port()
    server = Server(exe, port, use_valgrind=True)

    try:
        a = Client(port, 0)
        b = Client(port, 1)

        time.sleep(0.2)
        a.clear(); b.clear()

        a.send("hello\n")
        b.expect_sub(b"client 0: hello\n", where="valgrind smoke")

        a.close(); b.close()
        time.sleep(0.2)
    finally:
        rc, stderr = server.stop()

    err = stderr.decode(errors="replace")

    if not _valgrind_summary_ok(err):
        print("\n=== Valgrind stderr ===\n")
        print(err)
        print("=======================\n")
        fail("Valgrind reported memory issues")

    if rc not in (0, 42, -15, -2):
        fail(f"valgrind smoke: unexpected returncode {rc}\n{err}")

    print("    OK (no leaks detected)")


# ──────────────────────────────────────────────────────────────────────────────
# Entry point
# ──────────────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test suite for mini_serv")
    parser.add_argument("exe",        help="Path to the server executable")
    parser.add_argument("--valgrind", action="store_true", help="Run under Valgrind")
    parser.add_argument("--port",     type=int, default=None, help="Use a fixed port")
    args = parser.parse_args()

    if args.port is not None:
        def _fixed_port_factory(p=args.port):
            _free_port(p)
            return p
        globals()["get_free_port"] = _fixed_port_factory
        print(f"[INFO] Using fixed port: {args.port}")

    check_source_rules("mini_serv.c")

    test_wrong_arg_count(args.exe)
    test_bind_conflict(args.exe)
    test_localhost_only(args.exe)
    test_arrived(args.exe)
    test_broadcast(args.exe)
    test_partial_message(args.exe)
    test_multi_message_packet(args.exe)
    test_left(args.exe)
    test_id_monotonicity(args.exe)
    test_lazy_client(args.exe)
    test_large_message(args.exe)
    test_stress(args.exe)
    test_rst_disconnect(args.exe)
    test_sigpipe_resistance(args.exe)
    test_recv_returns_zero(args.exe)
    test_select_not_busyloop(args.exe)
    test_no_stdout(args.exe)
    test_concurrent_connect_disconnect(args.exe)

    if args.valgrind:
        test_valgrind_smoke(args.exe)
    else:
        print("[SKIP] Valgrind disabled — pass --valgrind to check for memory leaks")

    print("\n[PASS] All tests passed")
