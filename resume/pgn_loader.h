// pgn_loader.h -- lee un .pgn y reconstruye el tablero jugada a jugada,
// sin pasar por Python ni por un .h generado. Aplica las jugadas SAN sobre
// un tablero real en C (enroque, captura al paso, promocion, y filtro de
// jaque para desambiguar cuando dos piezas del mismo tipo podrian llegar
// al mismo destino).
#ifndef PGN_LOADER_H
#define PGN_LOADER_H

#include <stdbool.h>

#define PGN_MAX_HEADER 128
#define PGN_MAX_SAN     16

typedef struct {
    char san[PGN_MAX_SAN];
    char board[8][9];   // board[0]=rank8 .. board[7]=rank1 (mismo formato
                         // que usaba BOARD_STATES: fila 0 es la de arriba,
                         // columnas a..h de izq a derecha, '.' = vacio)
    char sideToMove;     // 'w' o 'b': quien mueve DESPUES de esta jugada
} PgnPly;

typedef struct {
    char white[PGN_MAX_HEADER];
    char black[PGN_MAX_HEADER];
    char result[16];
    char event[PGN_MAX_HEADER];
    char date[32];

    PgnPly *plies;  // plies[0] = posicion inicial ("-- inicio --")
    int numPlies;
} PgnGame;

// Lee fileName (via LoadFileText de raylib, funciona igual en WASM con
// --preload-file que en nativo) y parsea cabeceras + movetext SAN completo.
// Devuelve false si no pudo leer el archivo. Si una jugada SAN individual no
// se puede resolver, tira un TraceLog(LOG_WARNING) y esa posicion queda
// igual a la anterior (no aborta el parseo del resto de la partida).
bool LoadPgnGame(const char *fileName, PgnGame *out);
void UnloadPgnGame(PgnGame *game);

#endif // PGN_LOADER_H
