#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "pgn_loader.h"
#include "video_export.h"

#define GAME_PGN_FILE "assets/game.pgn"

#define PIECE_ANIMATION_DURATION 0.4f

#define BOARD_HALF_EXTENT 0.24f
#define BOARD_SQUARE_SIZE  (BOARD_HALF_EXTENT * 2.0f / 8.0f)
#define BOARD_TOP_Y        0.0122f
#define BOARD_MODEL_SCALE 0.01f

#define COLOR_PIECE_WHITE (Color){ 235, 227, 210, 255 }
#define COLOR_PIECE_BLACK (Color){  42,  37,  34, 255 }
#define COLOR_BOARD_LIGHT (Color){ 222, 202, 165, 255 }
#define COLOR_BOARD_DARK  (Color){  92,  60,  40, 255 }

#define CAPTURE_OFFSET_X -0.40f
#define CAPTURE_OFFSET_Z_START 0.35f
#define CAPTURE_OFFSET_Z_STEP 0.06f

enum PieceType {
    PC_NONE = 0,
    PC_PAWN = 1, PC_KNIGHT = 2, PC_BISHOP = 3, PC_ROOK = 4, PC_QUEEN = 5, PC_KING = 6,
    PC_COUNT = 7
};

static float EaseOutCubic(float t) {
    if (t >= 1.0f) return 1.0f;
    float u = 1.0f - t;
    return 1.0f - (u * u * u);
}

typedef struct {
    enum PieceType type;
    bool isWhite;
    Vector3 startPos;
    Vector3 endPos;
    float duration;
    float elapsed;
    bool isCapture;
} PieceAnimation;

typedef struct {
    enum PieceType type;
    bool isWhite;
    Vector3 position;
} CapturedPiece;

typedef struct {
    char board[8][9];
    PieceAnimation animations[32];
    int numAnimations;
    CapturedPiece captured[32];
    int numCaptured;
    bool moveInProgress;
} VisualBoardState;

typedef struct {
    Model board;
    float squareSize;
    Vector3 originSquareCenter;
    Texture2D checkerTexture;
} BoardVisual;

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
    int currentPly;
    bool autoPlay;
    float autoPlayTimer;
    float autoPlaySpeed;
} GameState;

static BoardVisual g_board;
static OrbitCamera g_cam;
static GameState g_state;
static PgnGame g_game;
static VisualBoardState g_visual;
static Model g_pieceModels[PC_COUNT];

static enum PieceType PieceTypeFromChar(char c) {
    switch (toupper((unsigned char)c)) {
        case 'P': return PC_PAWN;
        case 'N': return PC_KNIGHT;
        case 'B': return PC_BISHOP;
        case 'R': return PC_ROOK;
        case 'Q': return PC_QUEEN;
        case 'K': return PC_KING;
        default: return PC_NONE;
    }
}

static bool IsWhitePieceChar(char c) {
    return c >= 'A' && c <= 'Z';
}

static void LogModelTextures(const char* label, Model* model) {
    TraceLog(LOG_INFO, "=== %s: %d material(es) ===", label, model->materialCount);
    for (int i = 0; i < model->materialCount; i++) {
        Texture2D tex = model->materials[i].maps[MATERIAL_MAP_DIFFUSE].texture;
        Color col = model->materials[i].maps[MATERIAL_MAP_DIFFUSE].color;
        TraceLog(LOG_INFO, "  material[%d]: texture.id=%d (%dx%d)  baseColor=(%d,%d,%d,%d)",
                 i, tex.id, tex.width, tex.height, col.r, col.g, col.b, col.a);
    }
}

static void SetupLighting(void) {
    g_lightShader = LoadShaderFromMemory(g_lightVS, g_lightFS);
    g_locLightDir = GetShaderLocation(g_lightShader, "lightDir");
    g_locViewPos  = GetShaderLocation(g_lightShader, "viewPos");

    Vector3 lightDir = Vector3Normalize((Vector3){ -0.4f, -1.0f, -0.35f });
    SetShaderValue(g_lightShader, g_locLightDir, &lightDir, SHADER_UNIFORM_VEC3);
}

static Texture2D MakeBoardCheckerTexture(void) {
    const int texSize = 256;
    const int cell = texSize / 8;
    Image img = GenImageChecked(texSize, texSize, cell, cell, COLOR_BOARD_LIGHT, COLOR_BOARD_DARK);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

static Vector3 SquareToWorld(int file, int rank) {
    float x = g_board.originSquareCenter.x + file * g_board.squareSize;
    float z = g_board.originSquareCenter.z - rank * g_board.squareSize;
    return (Vector3){ x, g_board.originSquareCenter.y, z };
}

static void SetupBoardVisual(void) {
    g_board.board = LoadModel("assets/chess_board.glb");
    LogModelTextures("chess_board.glb", &g_board.board);

    g_board.checkerTexture = MakeBoardCheckerTexture();
    for (int i = 0; i < g_board.board.materialCount; i++) {
        Material *mat = &g_board.board.materials[i];
        if (mat->maps[MATERIAL_MAP_DIFFUSE].texture.id == 0) {
            mat->maps[MATERIAL_MAP_DIFFUSE].texture = g_board.checkerTexture;
            mat->maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
        }
        mat->shader = g_lightShader;
    }

    g_board.squareSize = BOARD_SQUARE_SIZE;
    float half = BOARD_HALF_EXTENT - g_board.squareSize * 0.5f;
    g_board.originSquareCenter = (Vector3){ -half, BOARD_TOP_Y, half };
}

static void LoadPieceModels(void) {
    const char *files[] = {
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
                mat->maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
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

static void DrawPiece(enum PieceType type, Vector3 pos, bool isWhite) {
    if (type == PC_NONE) return;
    Color tint = isWhite ? COLOR_PIECE_WHITE : COLOR_PIECE_BLACK;
    DrawModel(g_pieceModels[type], pos, 1.0f, tint);
}

static void InitVisualBoard(void) {
    g_visual.numAnimations = 0;
    g_visual.numCaptured = 0;
    g_visual.moveInProgress = false;
    memcpy(g_visual.board, g_game.plies[0].board, sizeof(g_visual.board));
}

static bool FindMovedPiece(const char prevBoard[8][9], const char newBoard[8][9],
                           int *fromFile, int *fromRank, int *toFile, int *toRank) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char prevPiece = prevBoard[row][col];
            char newPiece = newBoard[row][col];
            if (prevPiece != '.' && newPiece == '.') {
                *fromFile = col;
                *fromRank = 7 - row;
                for (int row2 = 0; row2 < 8; row2++) {
                    for (int col2 = 0; col2 < 8; col2++) {
                        if (newBoard[row2][col2] == prevPiece && prevBoard[row2][col2] == '.') {
                            *toFile = col2;
                            *toRank = 7 - row2;
                            return true;
                        }
                    }
                }
            }
        }
    }
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char prevPiece = prevBoard[row][col];
            char newPiece = newBoard[row][col];
            if (prevPiece != newPiece && prevPiece != '.' && newPiece != '.') {
                *toFile = col;
                *toRank = 7 - row;
                for (int row2 = 0; row2 < 8; row2++) {
                    for (int col2 = 0; col2 < 8; col2++) {
                        if (prevBoard[row2][col2] == newPiece && newBoard[row2][col2] == '.') {
                            *fromFile = col2;
                            *fromRank = 7 - row2;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

static void RebuildCaptures(int upToPly) {
    g_visual.numCaptured = 0;
    for (int i = 1; i <= upToPly; i++) {
        int fromFile, fromRank, toFile, toRank;
        if (FindMovedPiece(g_game.plies[i-1].board, g_game.plies[i].board,
                           &fromFile, &fromRank, &toFile, &toRank)) {
            char captured = g_game.plies[i-1].board[7-toRank][toFile];
            if (captured != '.') {
                enum PieceType type = PieceTypeFromChar(captured);
                bool isWhite = IsWhitePieceChar(captured);
                if (g_visual.numCaptured < 32) {
                    g_visual.captured[g_visual.numCaptured].type = type;
                    g_visual.captured[g_visual.numCaptured].isWhite = isWhite;
                    int idx = g_visual.numCaptured;
                    g_visual.captured[g_visual.numCaptured].position = (Vector3){
                        CAPTURE_OFFSET_X,
                        BOARD_TOP_Y,
                        CAPTURE_OFFSET_Z_START - idx * CAPTURE_OFFSET_Z_STEP
                    };
                    g_visual.numCaptured++;
                }
            }
        }
    }
}

static void AnimateMoveForward(int fromPly, int toPly) {
    if (toPly != fromPly + 1) {
        memcpy(g_visual.board, g_game.plies[toPly].board, sizeof(g_visual.board));
        RebuildCaptures(toPly);
        g_visual.numAnimations = 0;
        g_visual.moveInProgress = false;
        return;
    }

    int fromFile, fromRank, toFile, toRank;
    if (!FindMovedPiece(g_game.plies[fromPly].board, g_game.plies[toPly].board,
                        &fromFile, &fromRank, &toFile, &toRank)) {
        memcpy(g_visual.board, g_game.plies[toPly].board, sizeof(g_visual.board));
        RebuildCaptures(toPly);
        g_visual.numAnimations = 0;
        g_visual.moveInProgress = false;
        return;
    }

    char piece = g_game.plies[fromPly].board[7-fromRank][fromFile];
    enum PieceType type = PieceTypeFromChar(piece);
    bool isWhite = IsWhitePieceChar(piece);

    g_visual.numAnimations = 0;

    PieceAnimation *anim = &g_visual.animations[g_visual.numAnimations++];
    anim->type = type;
    anim->isWhite = isWhite;
    anim->startPos = SquareToWorld(fromFile, fromRank);
    anim->endPos = SquareToWorld(toFile, toRank);
    anim->duration = PIECE_ANIMATION_DURATION;
    anim->elapsed = 0.0f;
    anim->isCapture = false;

    char captured = g_game.plies[fromPly].board[7-toRank][toFile];
    if (captured != '.') {
        enum PieceType capturedType = PieceTypeFromChar(captured);
        bool capturedIsWhite = IsWhitePieceChar(captured);
        int idx = g_visual.numCaptured;
        Vector3 captureEndPos = (Vector3){
            CAPTURE_OFFSET_X,
            BOARD_TOP_Y,
            CAPTURE_OFFSET_Z_START - idx * CAPTURE_OFFSET_Z_STEP
        };

        PieceAnimation *captureAnim = &g_visual.animations[g_visual.numAnimations++];
        captureAnim->type = capturedType;
        captureAnim->isWhite = capturedIsWhite;
        captureAnim->startPos = SquareToWorld(toFile, toRank);
        captureAnim->endPos = captureEndPos;
        captureAnim->duration = PIECE_ANIMATION_DURATION;
        captureAnim->elapsed = 0.0f;
        captureAnim->isCapture = true;
    }

    memcpy(g_visual.board, g_game.plies[toPly].board, sizeof(g_visual.board));
    g_visual.moveInProgress = true;
}

static void AnimateMoveBackward(int fromPly, int toPly) {
    memcpy(g_visual.board, g_game.plies[toPly].board, sizeof(g_visual.board));
    RebuildCaptures(toPly);
    g_visual.numAnimations = 0;
    g_visual.moveInProgress = false;
}

static void UpdateAnimations(float dt) {
    if (g_visual.numAnimations == 0) {
        g_visual.moveInProgress = false;
        return;
    }

    for (int i = 0; i < g_visual.numAnimations; i++) {
        PieceAnimation *anim = &g_visual.animations[i];
        anim->elapsed += dt;
        if (anim->elapsed >= anim->duration && anim->isCapture) {
            if (g_visual.numCaptured < 32) {
                g_visual.captured[g_visual.numCaptured].type = anim->type;
                g_visual.captured[g_visual.numCaptured].isWhite = anim->isWhite;
                g_visual.captured[g_visual.numCaptured].position = anim->endPos;
                g_visual.numCaptured++;
            }
        }
    }

    int writeIndex = 0;
    for (int i = 0; i < g_visual.numAnimations; i++) {
        PieceAnimation *anim = &g_visual.animations[i];
        if (anim->elapsed < anim->duration) {
            if (writeIndex != i) {
                g_visual.animations[writeIndex] = *anim;
            }
            writeIndex++;
        }
    }
    g_visual.numAnimations = writeIndex;

    if (g_visual.numAnimations == 0) {
        g_visual.moveInProgress = false;
    }
}

static void DrawVisualBoard(void) {
    for (int row = 0; row < 8; row++) {
        const char* rowStr = g_visual.board[row];
        int rank = 7 - row;
        for (int col = 0; col < 8; col++) {
            char c = rowStr[col];
            if (c == '.') continue;

            bool isAnimating = false;
            Vector3 pos = SquareToWorld(col, rank);
            for (int i = 0; i < g_visual.numAnimations; i++) {
                PieceAnimation *anim = &g_visual.animations[i];
                if (anim->isCapture) continue;
                if (Vector3Equals(pos, anim->startPos) || Vector3Equals(pos, anim->endPos)) {
                    isAnimating = true;
                    break;
                }
            }
            if (!isAnimating) {
                enum PieceType type = PieceTypeFromChar(c);
                bool isWhite = IsWhitePieceChar(c);
                DrawPiece(type, pos, isWhite);
            }
        }
    }

    for (int i = 0; i < g_visual.numAnimations; i++) {
        PieceAnimation *anim = &g_visual.animations[i];
        float t = anim->elapsed / anim->duration;
        if (t > 1.0f) t = 1.0f;
        t = EaseOutCubic(t);
        Vector3 currentPos = Vector3Lerp(anim->startPos, anim->endPos, t);
        DrawPiece(anim->type, currentPos, anim->isWhite);
    }

    for (int i = 0; i < g_visual.numCaptured; i++) {
        CapturedPiece *cp = &g_visual.captured[i];
        DrawPiece(cp->type, cp->position, cp->isWhite);
    }
}

static void SetupCamera(void) {
    g_cam.angleH = 25.0f;
    g_cam.angleV = 35.0f;
    g_cam.distance = 0.9f;
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

static void GoToPly(int ply) {
    if (ply < 0) ply = 0;
    if (ply > g_game.numPlies - 1) ply = g_game.numPlies - 1;
    if (!g_visual.moveInProgress && ply != g_state.currentPly) {
        if (ply > g_state.currentPly) {
            AnimateMoveForward(g_state.currentPly, ply);
        } else {
            AnimateMoveBackward(g_state.currentPly, ply);
        }
        g_state.currentPly = ply;
    }
}

static void UpdateInput(float dt) {
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE)) GoToPly(g_state.currentPly + 1);
    if (IsKeyPressed(KEY_LEFT)) GoToPly(g_state.currentPly - 1);
    if (IsKeyPressed(KEY_HOME)) GoToPly(0);
    if (IsKeyPressed(KEY_END)) GoToPly(g_game.numPlies - 1);
    if (IsKeyPressed(KEY_P)) g_state.autoPlay = !g_state.autoPlay;
    if (IsKeyPressed(KEY_V)) VideoExportStart(GetScreenWidth(), GetScreenHeight());
    if (IsKeyPressed(KEY_X)) VideoExportStop();

    if (g_state.autoPlay) {
        g_state.autoPlayTimer += dt;
        if (g_state.autoPlayTimer >= g_state.autoPlaySpeed) {
            g_state.autoPlayTimer = 0.0f;
            if (g_state.currentPly < g_game.numPlies - 1) GoToPly(g_state.currentPly + 1);
            else g_state.autoPlay = false;
        }
    }
}

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

    if (g_visual.moveInProgress) {
        DrawText("[ANIMATING...]", 10, 190, 14, (Color){ 255, 150, 0, 255 });
    }

    if (g_state.autoPlay) {
        DrawText("AUTOPLAY", GetScreenWidth() - 130, 10, 18, (Color){ 200, 40, 40, 255 });
    }

    if (VideoExportIsRecording()) {
        DrawText(TextFormat("REC %d frames", VideoExportGetFrameCount()), 
                 GetScreenWidth() - 180, GetScreenHeight() - 50, 18, (Color){ 255, 40, 40, 255 });
    }
    DrawText("V: rec start   X: rec stop", GetScreenWidth() - 230, GetScreenHeight() - 25, 12, DARKGRAY);

    DrawFPS(GetScreenWidth() - 90, GetScreenHeight() - 25);
}

static void UpdateDrawFrame(void) {
    float dt = GetFrameTime();

    UpdateInput(dt);
    UpdateAnimations(dt);
    UpdateOrbitCamera(dt);
    SetShaderValue(g_lightShader, g_locViewPos, &g_cam.camera.position, SHADER_UNIFORM_VEC3);

    BeginDrawing();
    ClearBackground((Color){ 30, 30, 34, 255 });

    BeginMode3D(g_cam.camera);
        DrawModel(g_board.board, Vector3Zero(), BOARD_MODEL_SCALE, WHITE);
        DrawVisualBoard();
    EndMode3D();

    DrawUI();
    EndDrawing();

    if (VideoExportIsRecording()) {
        VideoExportCaptureFrame();
    }
}


int main(void) {
    #ifdef PLATFORM_WEB
        int screenWidth = 1280;
        int screenHeight = 720;
    #else
        const int screenWidth = 1280;
        const int screenHeight = 720;
    #endif

    InitWindow(screenWidth, screenHeight, "Chess Viewer - WASM");
    SetTargetFPS(60);
    VideoExportInit();

    SetupLighting();
    if (!LoadPgnGame(GAME_PGN_FILE, &g_game)) {
        TraceLog(LOG_ERROR, "No se pudo cargar %s", GAME_PGN_FILE);
    }
    SetupBoardVisual();
    LoadPieceModels();
    SetupCamera();
    InitVisualBoard();

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
    VideoExportCleanup();
    CloseWindow();
    return 0;
}