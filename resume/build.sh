#!/bin/bash
set -e

RAYLIB_SRC="${RAYLIB_SRC:-../raylib/src}"
OUTPUT_FILE="index.html"

echo "[Build] Chess Viewer - WASM + Video Export"
echo "=========================================="

# ============================================================================
# 1. VALIDAR RAYLIB
# ============================================================================
if [ ! -f "$RAYLIB_SRC/libraylib.a" ]; then
    echo "❌ Error: No se encontró raylib en $RAYLIB_SRC/libraylib.a"
    echo ""
    echo "Solución: Compila raylib primero"
    echo "  cd ../raylib/src"
    echo "  emmake make PLATFORM=PLATFORM_WEB -j4"
    exit 1
fi
echo "✓ Raylib encontrado: $RAYLIB_SRC/libraylib.a"

# ============================================================================
# 2. CREAR DIRECTORIOS
# ============================================================================
echo "[Build] Creando directorios..."
mkdir -p assets
mkdir -p ffmpeg

# ============================================================================
# 3. COPIAR ASSETS
# ============================================================================
echo "[Build] Copiando assets..."
for file in chess_board.glb pawn.glb knight.glb bishop.glb rook.glb queen.glb king.glb; do
    if [ -f "$file" ]; then
        cp "$file" assets/ 2>/dev/null && echo "  ✓ $file"
    fi
done

# ============================================================================
# 4. FFMPEG (OPCIONAL - Para video export)
# ============================================================================
echo "[Build] Verificando FFmpeg.wasm..."

if [ -d "../ffmpeg" ]; then
    echo "[Build] Copiando FFmpeg.wasm local..."
    cp -r ../ffmpeg/* ffmpeg/ 2>/dev/null || true
    echo "  ✓ FFmpeg local copiado (grabación más rápida)"
elif [ -f "/usr/local/ffmpeg/ffmpeg.min.js" ]; then
    echo "[Build] Copiando FFmpeg.wasm del sistema..."
    cp /usr/local/ffmpeg/* ffmpeg/ 2>/dev/null || true
    echo "  ✓ FFmpeg del sistema copiado"
else
    echo "  ⚠ FFmpeg.wasm NO encontrado en ../ffmpeg o /usr/local/ffmpeg"
    echo "  ℹ Se usará CDN (https://cdn.jsdelivr.net/npm/@ffmpeg/ffmpeg)"
    echo "  ℹ Primera carga puede tardar 30-60 segundos"
fi

# ============================================================================
# 5. COMPILAR CON EMSCRIPTEN
# ============================================================================
echo ""
echo "[Build] Compilando con Emscripten..."
echo "  Archivos: chess_viewer.c, pgn_loader.c"
echo "  Raylib:   $RAYLIB_SRC/libraylib.a"
echo "  Output:   $OUTPUT_FILE (+ assets + ffmpeg)"
echo ""

emcc chess_viewer.c pgn_loader.c \
    -o "$OUTPUT_FILE" \
    -Os -Wall -Wextra \
    -DPLATFORM_WEB \
    -I. -I"$RAYLIB_SRC" \
    "$RAYLIB_SRC/libraylib.a" \
    -s USE_GLFW=3 \
    -s ASYNCIFY \
    -s TOTAL_MEMORY=67108864 \
    -s FORCE_FILESYSTEM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    --preload-file assets \
    --shell-file shell.html

# ============================================================================
# 6. RESULTADOS
# ============================================================================
echo ""
echo "=========================================="
echo "✓ ¡COMPILACIÓN EXITOSA!"
echo "=========================================="
echo ""
echo "Archivos generados:"
ls -lh "$OUTPUT_FILE" "$(basename $OUTPUT_FILE .html).js" 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
echo ""

echo "Para servir la aplicación:"
echo "  python3 -m http.server 8000"
echo "  (o usar: npx http-server)"
echo ""
echo "Luego abre en navegador:"
echo "  http://localhost:8000"
echo ""

echo "Controles en la aplicación:"
echo "  V              - Iniciar grabación de video"
echo "  X              - Detener grabación (descarga MP4)"
echo "  P              - Autoplay (reproducir partida automáticamente)"
echo "  ← / → / SPACE  - Navegar movimientos"
echo "  HOME / END     - Ir a inicio/fin"
echo "  Q/E            - Rotar cámara horizontal"
echo "  R/F            - Rotar cámara vertical"
echo "  W/S            - Zoom in/out"
echo "  A/D            - Mover cámara lateral"
echo ""

echo "Video Export:"
echo "  📹 Presiona V para grabar"
echo "  ⏹️  Presiona X para detener y descargar MP4"
echo "  ℹ️  Ver consola (F12) para logs de conversión"
echo ""
