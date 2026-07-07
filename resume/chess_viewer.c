// chess_viewer.c -- visualizador minimo de partidas de ajedrez con raylib (WASM)
//
// Build (ver build.sh):
//   emcc chess_viewer.c -o index.html \
//     -Os -Wall -DPLATFORM_WEB \
//     -I. -Iraylib/src -Lraylib/src -lraylib \
//     -s USE_GLFW=3 -s ASYNCIFY \
//     --preload-file assets \
//     -s TOTAL_MEMORY=67108864 \
//     --shell-file shell.html
//
// Controles:
//   -> / Espacio : jugada siguiente
//   <-           : jugada anterior
//   Home / End   : ir al inicio / final de la partida
//   P            : autoplay on/off
//   Q/E R/F W/S A/D : orbitar / zoom / paneo camara

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "pgn_loader.h"

// Poné aca tu .pgn (dentro de assets/ para que --preload-file lo empaquete).
#define GAME_PGN_FILE "assets/game.pgn"

// ---------------------------------------------------------------------------
// Config del tablero: chess_board.glb NO trae los ejes corregidos (viene de
// un FBX sin re-orientar), asi que su "arriba" real es el eje Z local, no Y.
// Las mallas de casillas ocupan x:[-0.24,0.24] y:[-0.24,0.24] z:[-0.0022,0.0122]
// en espacio local -- tablero de 0.48m (8 casillas de 0.06m) plano en XY, con
// Z como grosor/altura. Para que quede plano y con Y-arriba en raylib le
// aplicamos una rotacion de -90 grados en X al Model.transform (raylib la
// aplica sola en cada DrawModel, no hay que tocar los vertices).
// ---------------------------------------------------------------------------
#define BOARD_HALF_EXTENT 0.24f
#define BOARD_SQUARE_SIZE  (BOARD_HALF_EXTENT * 2.0f / 8.0f)   // 0.06 m
#define BOARD_TOP_Y        0.0122f                              // cara superior de las casillas, tras rotar

// chess_board.glb viene modelado en "centimetros" (footprint real de las
// casillas: -24..24 unidades, marco: -30.5..30.5). Todas las constantes de
// arriba (BOARD_HALF_EXTENT, BOARD_SQUARE_SIZE, BOARD_TOP_Y) ya estan
// calculadas asumiendo el modelo escalado x0.01 a metros -- por eso hay que
// aplicar ese factor al dibujarlo, si no el tablero sale 100x mas grande de
// lo que el resto del codigo (y las piezas, que si vienen ya en metros)
// espera.
#define BOARD_MODEL_SCALE 0.01f

// ---------------------------------------------------------------------------
// Paleta con contraste real. OJO: nunca usar (0,0,0) puro para "negro" ni
// (255,255,255) puro para "blanco" en las piezas -- con el shader default de
// raylib (sin luz, solo texelColor*colDiffuse*tint) un material (0,0,0)
// sale liso y sin ningun matiz, no se distingue la forma de la pieza.
// ---------------------------------------------------------------------------
#define COLOR_PIECE_WHITE (Color){ 235, 227, 210, 255 } // marfil calido
#define COLOR_PIECE_BLACK (Color){  42,  37,  34, 255 } // "negro" carbon
#define COLOR_BOARD_LIGHT (Color){ 222, 202, 165, 255 } // casilla clara
#define COLOR_BOARD_DARK  (Color){  92,  60,  40, 255 } // casilla oscura

typedef struct {
    Model board;
    float squareSize;
    Vector3 originSquareCenter; // centro de la casilla file=0,rank=0
    Texture2D checkerTexture;   // generada en runtime, para unload al salir
} BoardVisual;

// -------------------- shader de iluminacion (GLSL ES 100 / WebGL1) --------
// El shader default de raylib no ilumina nada (solo tinte * textura), por
// eso todo se ve "plano". Este shader agrega difusa (Lambert) + un toque de
// specular para que las piezas se lean como solidos 3D y no como manchas.
static const char *g_lightVS =
    "#version 100\n"
    "attribute vec3 vertexPosition;\n"
    "attribute vec2 vertexTexCoord;\n"
    "attribute vec3 vertexNormal;\n"
    "attribute vec4 vertexColor;\n"
    "uniform mat4 mvp;\n"
    "uniform mat4 matModel;\n"
    "uniform mat4 matNormal;\n"
    "varying vec2 fragTexCoord;\n"
    "varying vec4 fragColor;\n"
    "varying vec3 fragNormal;\n"
    "varying vec3 fragPos;\n"
    "void main() {\n"
    "    fragTexCoord = vertexTexCoord;\n"
    "    fragColor = vertexColor;\n"
    "    fragNormal = normalize((matNormal * vec4(vertexNormal, 0.0)).xyz);\n"
    "    fragPos = (matModel * vec4(vertexPosition, 1.0)).xyz;\n"
    "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
    "}\n";

static const char *g_lightFS =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 fragTexCoord;\n"
    "varying vec4 fragColor;\n"
    "varying vec3 fragNormal;\n"
    "varying vec3 fragPos;\n"
    "uniform sampler2D texture0;\n"
    "uniform vec4 colDiffuse;\n"
    "uniform vec3 lightDir;\n"
    "uniform vec3 viewPos;\n"
    "void main() {\n"
    "    vec4 texelColor = texture2D(texture0, fragTexCoord);\n"
    "    vec3 n = normalize(fragNormal);\n"
    "    vec3 l = normalize(-lightDir);\n"
    "    float diff = max(dot(n, l), 0.0);\n"
    "    float ambient = 0.5;\n"
    "    vec3 viewDir = normalize(viewPos - fragPos);\n"
    "    vec3 halfVec = normalize(l + viewDir);\n"
    "    float spec = pow(max(dot(n, halfVec), 0.0), 24.0) * 0.25;\n"
    "    float lightAmount = clamp(ambient + diff * 0.6, 0.0, 1.0);\n"
    "    vec4 base = texelColor * colDiffuse * fragColor;\n"
    "    vec3 finalColor = base.rgb * lightAmount + vec3(spec);\n"
    "    gl_FragColor = vec4(finalColor, base.a);\n"
    "}\n";

static Shader g_lightShader;
static int g_locLightDir;
static int g_locViewPos;

typedef struct {
    Camera3D camera;
    float angleH, angleV, distance;
    Vector3 targetOffset;
} OrbitCamera;

typedef struct {
    int currentPly;      // 0 .. g_game.numPlies-1
    bool autoPlay;
    float autoPlayTimer;
    float autoPlaySpeed; // segundos por jugada
} GameState;

static BoardVisual g_board;
static OrbitCamera g_cam;
static GameState g_state;
static PgnGame g_game;

// -------------------- utilidades de tablero --------------------

// Imprime, para cada material del modelo, si tiene textura real (id>0, con
// ancho/alto) o si solo tiene un color plano (texture.id==0, w=0 h=0 -> no
// hay ninguna imagen en el archivo, y por tanto nada que asignar). Esto sale
// en la consola del navegador (F12) al arrancar.
static void LogModelTextures(const char* label, Model* model) {
    TraceLog(LOG_INFO, "=== %s: %d material(es) ===", label, model->materialCount);
    for (int i = 0; i < model->materialCount; i++) {
        Texture2D tex = model->materials[i].maps[MATERIAL_MAP_DIFFUSE].texture;
        Color col = model->materials[i].maps[MATERIAL_MAP_DIFFUSE].color;
        TraceLog(LOG_INFO, "  material[%d]: texture.id=%d (%dx%d)  baseColor=(%d,%d,%d,%d)",
                 i, tex.id, tex.width, tex.height, col.r, col.g, col.b, col.a);
    }
}

// Carga el shader de iluminacion y fija la direccion de luz (no cambia).
// viewPos se actualiza cada frame en UpdateDrawFrame porque la camara orbita.
static void SetupLighting(void) {
    g_lightShader = LoadShaderFromMemory(g_lightVS, g_lightFS);
    g_locLightDir = GetShaderLocation(g_lightShader, "lightDir");
    g_locViewPos  = GetShaderLocation(g_lightShader, "viewPos");

    Vector3 lightDir = Vector3Normalize((Vector3){ -0.4f, -1.0f, -0.35f });
    SetShaderValue(g_lightShader, g_locLightDir, &lightDir, SHADER_UNIFORM_VEC3);
}

// Genera (sin archivos externos) un checker 8x8 para el tablero. Asume que
// el UV del tablero cubre 0..1 en toda la superficie; si tu chess_board.glb
// tiene un material por casilla en vez de uno solo, esto no va a alinear
// perfecto -- revisa cuantos materiales reporta LogModelTextures() y avisame.
static Texture2D MakeBoardCheckerTexture(void) {
    const int texSize = 256;
    const int cell = texSize / 8; // 8 casillas por lado
    Image img = GenImageChecked(texSize, texSize, cell, cell, COLOR_BOARD_LIGHT, COLOR_BOARD_DARK);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

static Vector3 SquareToWorld(int file, int rank) {
    // file: 0=a .. 7=h   rank: 0=1 .. 7=8
    float x = g_board.originSquareCenter.x + file * g_board.squareSize;
    float z = g_board.originSquareCenter.z - rank * g_board.squareSize;
    return (Vector3){ x, g_board.originSquareCenter.y, z };
}

static void SetupBoardVisual(void) {
    g_board.board = LoadModel("assets/chess_board.glb");
    LogModelTextures("chess_board.glb", &g_board.board);

    // Solo pisamos materiales que no traigan textura real (texture.id==0):
    // si algun dia cargas un chess_board.glb con textura de madera de verdad,
    // esto la respeta y no la tapa con el checker generado.
    g_board.checkerTexture = MakeBoardCheckerTexture();
    for (int i = 0; i < g_board.board.materialCount; i++) {
        Material *mat = &g_board.board.materials[i];
        if (mat->maps[MATERIAL_MAP_DIFFUSE].texture.id == 0) {
            mat->maps[MATERIAL_MAP_DIFFUSE].texture = g_board.checkerTexture;
            mat->maps[MATERIAL_MAP_DIFFUSE].color = WHITE; // que no tinte el checker
        }
        mat->shader = g_lightShader;
    }

    g_board.squareSize = BOARD_SQUARE_SIZE;

    float half = BOARD_HALF_EXTENT - g_board.squareSize * 0.5f; // 0.21
    g_board.originSquareCenter = (Vector3){ -half, BOARD_TOP_Y, half };
}

// -------------------- piezas (modelos GLB reales) --------------------

typedef enum { PC_NONE, PC_PAWN, PC_KNIGHT, PC_BISHOP, PC_ROOK, PC_QUEEN, PC_KING, PC_COUNT } PieceType;

// Un solo modelo por tipo (ya no hay w_/b_): el color de bando se aplica
// como tinte en DrawModel, no hay dos modelos ni dos materiales por pieza.
static Model g_pieceModels[PC_COUNT];

static PieceType PieceTypeFromChar(char c) {
    switch (c) {
        case 'p': case 'P': return PC_PAWN;
        case 'n': case 'N': return PC_KNIGHT;
        case 'b': case 'B': return PC_BISHOP;
        case 'r': case 'R': return PC_ROOK;
        case 'q': case 'Q': return PC_QUEEN;
        case 'k': case 'K': return PC_KING;
        default: return PC_NONE;
    }
}

static bool IsWhitePieceChar(char c) {
    return c >= 'A' && c <= 'Z';
}

// Los .glb de piezas ya vienen orientados Y-arriba y a escala real (base en
// y=0, footprint ~0.03m, que cabe holgado en una casilla de 0.06m). Como
// ahora es un solo modelo compartido por ambos bandos, el material queda
// neutro (WHITE) y el color real (marfil/carbon) se aplica como tinte en
// cada DrawModel segun de que bando sea la pieza -- si el material quedara
// pisado con un color fijo aca, el tinte se multiplicaria sobre ese color y
// blancas/negras saldrian mal.
static void LoadPieceModels(void) {
    const char* files[PC_COUNT] = {
        NULL, "assets/pawn.glb", "assets/knight.glb", "assets/bishop.glb",
        "assets/rook.glb", "assets/queen.glb", "assets/king.glb",
    };

    for (int type = PC_PAWN; type < PC_COUNT; type++) {
        g_pieceModels[type] = LoadModel(files[type]);
        LogModelTextures(files[type], &g_pieceModels[type]);

        Model *m = &g_pieceModels[type];
        for (int i = 0; i < m->materialCount; i++) {
            Material *mat = &m->materials[i];
            if (mat->maps[MATERIAL_MAP_DIFFUSE].texture.id == 0) {
                mat->maps[MATERIAL_MAP_DIFFUSE].color = WHITE; // neutro: el tinte hace el color real
            }
            mat->shader = g_lightShader;
        }
    }
}

static void UnloadPieceModels(void) {
    for (int type = PC_PAWN; type < PC_COUNT; type++) {
        UnloadModel(g_pieceModels[type]);
    }
}

static void DrawPiece(PieceType type, Vector3 pos, bool isWhite) {
    if (type == PC_NONE) return;
    Color tint = isWhite ? COLOR_PIECE_WHITE : COLOR_PIECE_BLACK;
    DrawModel(g_pieceModels[type], pos, 1.0f, tint);
}

static void DrawPosition(int ply) {
    for (int row = 0; row < 8; row++) {
        const char* rowStr = g_game.plies[ply].board[row];
        int rank = 7 - row; // fila 0 del string = rank8
        for (int col = 0; col < 8; col++) {
            char c = rowStr[col];
            if (c == '.') continue;
            PieceType type = PieceTypeFromChar(c);
            bool isWhite = IsWhitePieceChar(c);
            Vector3 pos = SquareToWorld(col, rank);
            DrawPiece(type, pos, isWhite);
        }
    }
}

// -------------------- camara orbital --------------------

static void SetupCamera(void) {
    g_cam.angleH = 25.0f;
    g_cam.angleV = 35.0f;
    g_cam.distance = 0.9f; // tablero real mide ~0.48m de lado, la distancia debe ir a juego
    g_cam.targetOffset = (Vector3){ 0, 0, 0 };

    g_cam.camera.up = (Vector3){ 0, 1, 0 };
    g_cam.camera.fovy = 45.0f;
    g_cam.camera.projection = CAMERA_PERSPECTIVE;
}

static void UpdateOrbitCamera(float dt) {
    if (IsKeyDown(KEY_Q)) g_cam.angleH += 60.0f * dt;
    if (IsKeyDown(KEY_E)) g_cam.angleH -= 60.0f * dt;
    if (IsKeyDown(KEY_R)) g_cam.angleV += 45.0f * dt;
    if (IsKeyDown(KEY_F)) g_cam.angleV -= 45.0f * dt;
    g_cam.angleV = Clamp(g_cam.angleV, 10.0f, 80.0f);

    if (IsKeyDown(KEY_W)) g_cam.distance -= 0.6f * dt;
    if (IsKeyDown(KEY_S)) g_cam.distance += 0.6f * dt;
    g_cam.distance = Clamp(g_cam.distance, 0.35f, 3.0f);

    float radH = g_cam.angleH * DEG2RAD;
    float radV = g_cam.angleV * DEG2RAD;

    Vector3 forward = { cosf(radV) * sinf(radH), sinf(radV), cosf(radV) * cosf(radH) };
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){ 0, 1, 0 }));

    if (IsKeyDown(KEY_A)) g_cam.targetOffset = Vector3Add(g_cam.targetOffset, Vector3Scale(right, -0.3f * dt));
    if (IsKeyDown(KEY_D)) g_cam.targetOffset = Vector3Add(g_cam.targetOffset, Vector3Scale(right, 0.3f * dt));

    g_cam.camera.target = Vector3Add(g_board.originSquareCenter, g_cam.targetOffset);
    g_cam.camera.target.x += g_board.squareSize * 3.5f;
    g_cam.camera.target.z -= g_board.squareSize * 3.5f;

    g_cam.camera.position.x = g_cam.camera.target.x + g_cam.distance * cosf(radV) * sinf(radH);
    g_cam.camera.position.y = g_cam.camera.target.y + g_cam.distance * sinf(radV);
    g_cam.camera.position.z = g_cam.camera.target.z + g_cam.distance * cosf(radV) * cosf(radH);
}

// -------------------- navegacion de jugadas --------------------

static void GoToPly(int ply) {
    if (ply < 0) ply = 0;
    if (ply > g_game.numPlies - 1) ply = g_game.numPlies - 1;
    g_state.currentPly = ply;
}

static void UpdateInput(float dt) {
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE)) GoToPly(g_state.currentPly + 1);
    if (IsKeyPressed(KEY_LEFT)) GoToPly(g_state.currentPly - 1);
    if (IsKeyPressed(KEY_HOME)) GoToPly(0);
    if (IsKeyPressed(KEY_END)) GoToPly(g_game.numPlies - 1);
    if (IsKeyPressed(KEY_P)) g_state.autoPlay = !g_state.autoPlay;

    if (g_state.autoPlay) {
        g_state.autoPlayTimer += dt;
        if (g_state.autoPlayTimer >= g_state.autoPlaySpeed) {
            g_state.autoPlayTimer = 0.0f;
            if (g_state.currentPly < g_game.numPlies - 1) GoToPly(g_state.currentPly + 1);
            else g_state.autoPlay = false;
        }
    }
}

// -------------------- UI --------------------

static void DrawUI(void) {
    DrawText(TextFormat("%s vs %s  (%s)", g_game.white, g_game.black, g_game.result), 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("%s - %s", g_game.event, g_game.date), 10, 34, 14, GRAY);

    int moveNumber = (g_state.currentPly + 1) / 2;
    const char* sideStr = (g_game.plies[g_state.currentPly].sideToMove == 'w') ? "blancas juegan" : "negras juegan";
    DrawText(TextFormat("Jugada %d/%d: %s", g_state.currentPly, g_game.numPlies - 1, g_game.plies[g_state.currentPly].san),
              10, 60, 20, (Color){ 20, 90, 20, 255 });
    DrawText(TextFormat("Turno %d, %s", moveNumber, sideStr), 10, 84, 14, GRAY);

    DrawRectangle(10, 106, 300, 8, LIGHTGRAY);
    float progress = (float)g_state.currentPly / (float)(g_game.numPlies - 1);
    DrawRectangle(10, 106, (int)(300 * progress), 8, (Color){ 20, 140, 20, 255 });

    DrawText("<- / -> : jugada anterior/siguiente", 10, 130, 14, DARKGRAY);
    DrawText("ESPACIO : siguiente   P: autoplay   HOME/END: inicio/fin", 10, 148, 14, DARKGRAY);
    DrawText("Q/E R/F W/S A/D : orbitar camara", 10, 166, 14, DARKGRAY);

    if (g_state.autoPlay) {
        DrawText("AUTOPLAY", GetScreenWidth() - 130, 10, 18, (Color){ 200, 40, 40, 255 });
    }

    DrawFPS(GetScreenWidth() - 90, GetScreenHeight() - 25);
}

// -------------------- loop principal --------------------

static void UpdateDrawFrame(void) {
    float dt = GetFrameTime();

    UpdateInput(dt);
    UpdateOrbitCamera(dt);
    SetShaderValue(g_lightShader, g_locViewPos, &g_cam.camera.position, SHADER_UNIFORM_VEC3);

    BeginDrawing();
    ClearBackground((Color){ 30, 30, 34, 255 });

    BeginMode3D(g_cam.camera);
        DrawModel(g_board.board, Vector3Zero(), BOARD_MODEL_SCALE, WHITE);
        DrawPosition(g_state.currentPly);
    EndMode3D();

    DrawUI();
    EndDrawing();
}

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Chess Viewer - WASM");
    SetTargetFPS(60);

    SetupLighting();     // debe ir antes: los modelos necesitan g_lightShader ya cargado
    if (!LoadPgnGame(GAME_PGN_FILE, &g_game)) {
        TraceLog(LOG_ERROR, "No se pudo cargar %s", GAME_PGN_FILE);
    }
    SetupBoardVisual();
    LoadPieceModels();
    SetupCamera();

    g_state.currentPly = 0;
    g_state.autoPlay = false;
    g_state.autoPlayTimer = 0.0f;
    g_state.autoPlaySpeed = 1.0f;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    UnloadModel(g_board.board);
    UnloadPieceModels();
    UnloadTexture(g_board.checkerTexture);
    UnloadShader(g_lightShader);
    UnloadPgnGame(&g_game);
    CloseWindow();
    return 0;
}
