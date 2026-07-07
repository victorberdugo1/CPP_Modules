#!/usr/bin/env bash
# Build del visor de ajedrez para WASM con emscripten + raylib.
#
# Requisitos previos (una sola vez):
#   git clone --depth 1 https://github.com/emscripten-core/emsdk.git
#   cd emsdk && ./emsdk install latest && ./emsdk activate latest
#   source ./emsdk_env.sh        (o .\emsdk_env.ps1 en Windows)
#
#   git clone --depth 1 https://github.com/raysan5/raylib.git
#   cd raylib/src
#   emmake make PLATFORM=PLATFORM_WEB -j4
#
# Luego, desde esta carpeta:
#   ./build.sh
#   python3 -m http.server
#   abrir http://localhost:8000/index.html

set -e

RAYLIB_SRC="${RAYLIB_SRC:-../raylib/src}"

mkdir -p assets
cp -n chess_board.glb assets/ 2>/dev/null || true

emcc chess_viewer.c -o index.html \
    -Os -Wall -DPLATFORM_WEB \
    -I. -I"$RAYLIB_SRC" \
    "$RAYLIB_SRC/libraylib.a" \
    -s USE_GLFW=3 \
    -s ASYNCIFY \
    -s TOTAL_MEMORY=67108864 \
    -s FORCE_FILESYSTEM=1 \
    --preload-file assets \
    --shell-file shell.html

echo "Build OK -> index.html / index.js / index.wasm / index.data"
