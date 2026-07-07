// pgn_loader.c -- parser de PGN + aplicador de jugadas SAN, en C puro.
//
// Limitacion consciente: no hace deteccion de jaque/jaque mate por si mismo
// (no la necesita para reconstruir el tablero); SI usa una verificacion de
// "no dejar al propio rey en jaque" para desambiguar cuando dos piezas del
// mismo tipo podrian pseudo-legalmente llegar al mismo destino (caso de
// piezas clavadas). Para partidas reales exportadas por cualquier interfaz
// de ajedrez esto es suficiente: el SAN que ellas generan siempre incluye
// desambiguacion cuando hace falta.

#include "pgn_loader.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// -------------------- utilidades chicas --------------------

static char *StrDup(const char *s) {
    size_t n = strlen(s) + 1;
    char *r = (char*)malloc(n);
    memcpy(r, s, n);
    return r;
}

static int Sign(int x) { return (x > 0) - (x < 0); }

// -------------------- estado de tablero (motor minimo) --------------------
// Convencion interna: b[rank][file], rank 0 = fila 1 .. rank 7 = fila 8,
// file 0 = columna a .. file 7 = columna h. Mayusculas = blancas.

typedef struct {
    char b[8][8];
    char turn;              // 'w' o 'b'
    bool wk, wq, bk, bq;    // derechos de enroque restantes
    int epFile;             // -1 si no hay objetivo de captura al paso
} ChessState;

static void InitChessState(ChessState *st) {
    static const char *backWhite = "RNBQKBNR";
    static const char *backBlack = "rnbqkbnr";
    for (int f = 0; f < 8; f++) {
        st->b[0][f] = backWhite[f];
        st->b[1][f] = 'P';
        for (int r = 2; r < 6; r++) st->b[r][f] = '.';
        st->b[6][f] = 'p';
        st->b[7][f] = backBlack[f];
    }
    st->turn = 'w';
    st->wk = st->wq = st->bk = st->bq = true;
    st->epFile = -1;
}

// Vuelca el estado interno al formato de snapshot (fila 0 = rank8, como
// imprimia python-chess y como espera DrawPosition() en chess_viewer.c).
static void SnapshotBoard(ChessState *st, PgnPly *ply) {
    for (int row = 0; row < 8; row++) {
        int rank = 7 - row;
        for (int f = 0; f < 8; f++) ply->board[row][f] = st->b[rank][f];
        ply->board[row][8] = '\0';
    }
}

static bool PathClear(ChessState *st, int sf, int sr, int df, int dr) {
    int dx = Sign(df - sf), dy = Sign(dr - sr);
    int f = sf + dx, r = sr + dy;
    while (f != df || r != dr) {
        if (st->b[r][f] != '.') return false;
        f += dx; r += dy;
    }
    return true;
}

// pieceUpper: 'N','B','R','Q','K' (los peones se manejan aparte, tienen
// reglas de captura/avance distintas al resto).
static bool ReachesSquare(char pieceUpper, int sf, int sr, int df, int dr, ChessState *st) {
    int dx = df - sf, dy = dr - sr;
    int adx = abs(dx), ady = abs(dy);
    switch (pieceUpper) {
        case 'N': return (adx == 1 && ady == 2) || (adx == 2 && ady == 1);
        case 'B': return adx == ady && adx > 0 && PathClear(st, sf, sr, df, dr);
        case 'R': return ((adx == 0) != (ady == 0)) && PathClear(st, sf, sr, df, dr);
        case 'Q': return ((adx == ady && adx > 0) || ((adx == 0) != (ady == 0))) && PathClear(st, sf, sr, df, dr);
        case 'K': return adx <= 1 && ady <= 1 && (adx + ady > 0);
        default:  return false;
    }
}

static bool IsAttacked(ChessState *st, int tf, int tr, char byColor) {
    for (int r = 0; r < 8; r++) for (int f = 0; f < 8; f++) {
        char c = st->b[r][f];
        if (c == '.') continue;
        bool isWhite = (c >= 'A' && c <= 'Z');
        if ((isWhite ? 'w' : 'b') != byColor) continue;
        char up = toupper((unsigned char)c);
        if (up == 'P') {
            int dir = isWhite ? 1 : -1;
            if (r + dir == tr && (f - 1 == tf || f + 1 == tf)) return true;
        } else if (ReachesSquare(up, f, r, tf, tr, st)) {
            return true;
        }
    }
    return false;
}

static void FindKing(ChessState *st, char color, int *kf, int *kr) {
    char target = (color == 'w') ? 'K' : 'k';
    for (int r = 0; r < 8; r++) for (int f = 0; f < 8; f++) {
        if (st->b[r][f] == target) { *kf = f; *kr = r; return; }
    }
    *kf = *kr = -1;
}

static bool InCheck(ChessState *st, char color) {
    int kf, kr;
    FindKing(st, color, &kf, &kr);
    if (kf < 0) return false; // no deberia pasar en una partida real
    return IsAttacked(st, kf, kr, color == 'w' ? 'b' : 'w');
}

// Aplica una jugada SAN sobre st. Devuelve false si no pudo resolverla
// (formato raro / hueco en la notacion); en ese caso st queda sin tocar.
static bool ApplySan(ChessState *st, const char *sanRaw) {
    char san[PGN_MAX_SAN];
    snprintf(san, sizeof(san), "%s", sanRaw);

    int len = (int)strlen(san);
    while (len > 0 && (san[len-1] == '+' || san[len-1] == '#' ||
                        san[len-1] == '!' || san[len-1] == '?')) {
        san[--len] = '\0';
    }

    bool white = (st->turn == 'w');

    if (strcmp(san, "O-O") == 0 || strcmp(san, "0-0") == 0) {
        int rank = white ? 0 : 7;
        st->b[rank][4] = '.'; st->b[rank][6] = white ? 'K' : 'k';
        st->b[rank][7] = '.'; st->b[rank][5] = white ? 'R' : 'r';
        if (white) st->wk = st->wq = false; else st->bk = st->bq = false;
        st->epFile = -1;
        st->turn = white ? 'b' : 'w';
        return true;
    }
    if (strcmp(san, "O-O-O") == 0 || strcmp(san, "0-0-0") == 0) {
        int rank = white ? 0 : 7;
        st->b[rank][4] = '.'; st->b[rank][2] = white ? 'K' : 'k';
        st->b[rank][0] = '.'; st->b[rank][3] = white ? 'R' : 'r';
        if (white) st->wk = st->wq = false; else st->bk = st->bq = false;
        st->epFile = -1;
        st->turn = white ? 'b' : 'w';
        return true;
    }

    char promo = 0;
    char *eq = strchr(san, '=');
    if (eq && eq[1]) { promo = (char)toupper((unsigned char)eq[1]); *eq = '\0'; }

    char piece = 'P';
    const char *p = san;
    if (*p == 'K' || *p == 'Q' || *p == 'R' || *p == 'B' || *p == 'N') { piece = *p; p++; }

    len = (int)strlen(p);
    if (len < 2) return false;

    int destF = p[len-2] - 'a';
    int destR = p[len-1] - '1';
    if (destF < 0 || destF > 7 || destR < 0 || destR > 7) return false;

    char before[PGN_MAX_SAN];
    int beforeLen = len - 2;
    if (beforeLen < 0 || beforeLen >= (int)sizeof(before)) return false;
    memcpy(before, p, beforeLen);
    before[beforeLen] = '\0';

    bool capture = (strchr(before, 'x') != NULL);
    char disambig[4]; int dlen = 0;
    for (int i = 0; before[i] && dlen < 3; i++) if (before[i] != 'x') disambig[dlen++] = before[i];
    disambig[dlen] = '\0';

    int srcFile = -1, srcRank = -1;
    for (int i = 0; i < dlen; i++) {
        if (disambig[i] >= 'a' && disambig[i] <= 'h') srcFile = disambig[i] - 'a';
        else if (disambig[i] >= '1' && disambig[i] <= '8') srcRank = disambig[i] - '1';
    }

    int foundF = -1, foundR = -1;

    if (piece == 'P') {
        int dir = white ? 1 : -1;
        if (capture) {
            foundF = srcFile;
            foundR = destR - dir;
            if (foundF < 0) return false;
            if (st->b[destR][destF] == '.') {
                // no habia nada en el destino -> fue al paso; el peon
                // capturado esta en la misma fila del que capturo.
                st->b[foundR][destF] = '.';
            }
        } else {
            foundF = destF;
            char myPawn = white ? 'P' : 'p';
            int oneBack = destR - dir;
            if (oneBack >= 0 && oneBack < 8 && st->b[oneBack][destF] == myPawn) {
                foundR = oneBack;
            } else {
                int twoBack = destR - 2*dir;
                if (twoBack >= 0 && twoBack < 8 && st->b[twoBack][destF] == myPawn) foundR = twoBack;
            }
        }
    } else {
        char myPiece = white ? piece : (char)tolower((unsigned char)piece);
        // primer intento: filtrando pseudo-legales que dejarian al propio
        // rey en jaque (asi desambiguamos piezas clavadas).
        for (int r = 0; r < 8; r++) for (int f = 0; f < 8; f++) {
            if (st->b[r][f] != myPiece) continue;
            if (srcFile != -1 && f != srcFile) continue;
            if (srcRank != -1 && r != srcRank) continue;
            if (!ReachesSquare(piece, f, r, destF, destR, st)) continue;

            char savedSrc = st->b[r][f], savedDst = st->b[destR][destF];
            st->b[destR][destF] = savedSrc; st->b[r][f] = '.';
            bool illegal = InCheck(st, white ? 'w' : 'b');
            st->b[r][f] = savedSrc; st->b[destR][destF] = savedDst;
            if (illegal) continue;

            foundF = f; foundR = r;
        }
        // fallback: si el filtro de jaque no dejo ningun candidato (jugada
        // rara / posicion ya inconsistente), usar el primer pseudo-legal.
        if (foundF == -1) {
            for (int r = 0; r < 8; r++) for (int f = 0; f < 8; f++) {
                if (st->b[r][f] != myPiece) continue;
                if (srcFile != -1 && f != srcFile) continue;
                if (srcRank != -1 && r != srcRank) continue;
                if (!ReachesSquare(piece, f, r, destF, destR, st)) continue;
                foundF = f; foundR = r;
            }
        }
    }

    if (foundF == -1 || foundR == -1) return false;

    char movingPiece = st->b[foundR][foundF];
    st->b[foundR][foundF] = '.';
    st->b[destR][destF] = promo ? (white ? promo : (char)tolower((unsigned char)promo)) : movingPiece;

    if (movingPiece == 'K') st->wk = st->wq = false;
    if (movingPiece == 'k') st->bk = st->bq = false;
    if (movingPiece == 'R' && foundR == 0 && foundF == 0) st->wq = false;
    if (movingPiece == 'R' && foundR == 0 && foundF == 7) st->wk = false;
    if (movingPiece == 'r' && foundR == 7 && foundF == 0) st->bq = false;
    if (movingPiece == 'r' && foundR == 7 && foundF == 7) st->bk = false;
    if (destR == 0 && destF == 0) st->wq = false;
    if (destR == 0 && destF == 7) st->wk = false;
    if (destR == 7 && destF == 0) st->bq = false;
    if (destR == 7 && destF == 7) st->bk = false;

    st->epFile = -1;
    if (piece == 'P' && abs(destR - foundR) == 2) st->epFile = destF;

    st->turn = white ? 'b' : 'w';
    return true;
}

// -------------------- parseo de cabeceras --------------------

static void ParseHeaders(const char *pgn, PgnGame *g) {
    snprintf(g->white,  sizeof g->white,  "%s", "?");
    snprintf(g->black,  sizeof g->black,  "%s", "?");
    snprintf(g->result, sizeof g->result, "%s", "?");
    snprintf(g->event,  sizeof g->event,  "%s", "?");
    snprintf(g->date,   sizeof g->date,   "%s", "?");

    const char *p = pgn;
    while (*p == '[') {
        char key[64] = {0};
        char val[PGN_MAX_HEADER] = {0};
        if (sscanf(p, "[%63[^ ] \"%127[^\"]\"", key, val) == 2) {
            if (strcmp(key, "White") == 0)  snprintf(g->white,  sizeof g->white,  "%s", val);
            else if (strcmp(key, "Black") == 0)  snprintf(g->black,  sizeof g->black,  "%s", val);
            else if (strcmp(key, "Result") == 0) snprintf(g->result, sizeof g->result, "%s", val);
            else if (strcmp(key, "Event") == 0)  snprintf(g->event,  sizeof g->event,  "%s", val);
            else if (strcmp(key, "Date") == 0)   snprintf(g->date,   sizeof g->date,   "%s", val);
        }
        const char *nl = strchr(p, '\n');
        if (!nl) break;
        p = nl + 1;
    }
}

// -------------------- tokenizador de movetext --------------------

typedef struct { char **items; int count; int cap; } StrList;

static void SLPush(StrList *l, const char *s) {
    if (l->count == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 64;
        l->items = (char**)realloc(l->items, l->cap * sizeof(char*));
    }
    l->items[l->count++] = StrDup(s);
}

static bool IsResultToken(const char *t) {
    return strcmp(t, "1-0") == 0 || strcmp(t, "0-1") == 0 ||
           strcmp(t, "1/2-1/2") == 0 || strcmp(t, "*") == 0;
}

static bool IsMoveNumberToken(const char *t) {
    const char *p = t; int digits = 0;
    while (isdigit((unsigned char)*p)) { p++; digits++; }
    if (digits == 0) return false;
    while (*p == '.') p++;
    return *p == '\0';
}

// Recorre el movetext saltando comentarios {..}, comentarios ;.., variantes
// (..) (con anidamiento), codigos NAG $n, numeros de jugada y el resultado
// final, y devuelve solo los tokens SAN de jugadas reales.
static StrList TokenizeMovetext(const char *text) {
    StrList moves = {0};
    const char *p = text;
    char tok[PGN_MAX_SAN];
    int parenDepth = 0;

    while (*p) {
        char c = *p;
        if (parenDepth > 0) {
            if (c == '(') parenDepth++;
            else if (c == ')') parenDepth--;
            p++;
            continue;
        }
        if (c == '{') { while (*p && *p != '}') p++; if (*p) p++; continue; }
        if (c == ';') { while (*p && *p != '\n') p++; continue; }
        if (c == '(') { parenDepth = 1; p++; continue; }
        if (c == '$') { p++; while (isdigit((unsigned char)*p)) p++; continue; }
        if (isspace((unsigned char)c)) { p++; continue; }

        int tokLen = 0;
        while (*p && !isspace((unsigned char)*p) && *p != '{' && *p != '(' && *p != ';') {
            if (tokLen < (int)sizeof(tok) - 1) tok[tokLen++] = *p;
            p++;
        }
        tok[tokLen] = '\0';
        if (tokLen == 0) continue;
        if (IsMoveNumberToken(tok)) continue;
        if (IsResultToken(tok)) continue;

        SLPush(&moves, tok);
    }
    return moves;
}

// -------------------- API publica --------------------

bool LoadPgnGame(const char *fileName, PgnGame *out) {
    memset(out, 0, sizeof(*out));

    char *text = LoadFileText(fileName);
    if (!text) {
        TraceLog(LOG_WARNING, "PGN: no se pudo abrir '%s'", fileName);
        return false;
    }

    ParseHeaders(text, out);

    const char *body = text;
    while (*body == '[') {
        const char *nl = strchr(body, '\n');
        if (!nl) { body += strlen(body); break; }
        body = nl + 1;
    }

    StrList moves = TokenizeMovetext(body);

    out->numPlies = moves.count + 1;
    out->plies = (PgnPly*)malloc(sizeof(PgnPly) * out->numPlies);

    ChessState st;
    InitChessState(&st);

    snprintf(out->plies[0].san, sizeof(out->plies[0].san), "%s", "-- inicio --");
    SnapshotBoard(&st, &out->plies[0]);
    out->plies[0].sideToMove = st.turn;

    for (int i = 0; i < moves.count; i++) {
        bool ok = ApplySan(&st, moves.items[i]);
        PgnPly *ply = &out->plies[i+1];
        snprintf(ply->san, sizeof(ply->san), "%s", moves.items[i]);
        SnapshotBoard(&st, ply);
        ply->sideToMove = st.turn;
        if (!ok) {
            TraceLog(LOG_WARNING, "PGN: no pude resolver la jugada '%s' (ply %d); repito la posicion anterior",
                      moves.items[i], i + 1);
        }
        free(moves.items[i]);
    }
    free(moves.items);

    UnloadFileText(text);

    TraceLog(LOG_INFO, "PGN: '%s' -> %d jugadas cargadas (%s vs %s, %s)",
              fileName, moves.count, out->white, out->black, out->result);
    return true;
}

void UnloadPgnGame(PgnGame *game) {
    if (!game) return;
    free(game->plies);
    game->plies = NULL;
    game->numPlies = 0;
}
