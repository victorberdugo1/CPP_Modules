# Makefile - build WASM (emscripten + raylib) para el chess viewer
# Con descarga automática de ffmpeg.wasm precompilado

EMSDK_DIR  ?= emsdk
EMSDK_ENV  := $(EMSDK_DIR)/emsdk_env.sh
EMSDK_EMCC := $(EMSDK_DIR)/upstream/emscripten/emcc

RAYLIB_SRC      ?= raylib/src
RAYLIB_REPO_DIR := $(patsubst %/src,%,$(RAYLIB_SRC))
RAYLIB_LIB_SRC  := $(RAYLIB_SRC)/libraylib.web.a
RAYLIB_LIB      := libraylib.a
RAYLIB_CONFIG   := $(RAYLIB_SRC)/config.h

# === FFmpeg.wasm Configuration ===
FFMPEG_VERSION        := 0.12.10
FFMPEG_DIR            := ffmpeg
FFMPEG_WRAPPER_FILES  := ffmpeg.min.js ffmpeg.js
FFMPEG_CORE_FILES     := ffmpeg-core.js ffmpeg-core.wasm
FFMPEG_FILES          := $(FFMPEG_WRAPPER_FILES) $(FFMPEG_CORE_FILES)
FFMPEG_TARGETS        := $(addprefix $(FFMPEG_DIR)/,$(FFMPEG_FILES))
FFMPEG_WRAPPER_URL    := https://cdn.jsdelivr.net/npm/@ffmpeg/ffmpeg@$(FFMPEG_VERSION)/dist/umd
FFMPEG_CORE_URL       := https://cdn.jsdelivr.net/npm/@ffmpeg/core@$(FFMPEG_VERSION)/dist/umd

# === Compilation ===
EMCC := emcc

TARGET   := index
OUT_HTML := $(TARGET).html
OUT_JS   := $(TARGET).js
OUT_WASM := $(TARGET).wasm
OUT_DATA := $(TARGET).data
OUTPUTS  := $(OUT_HTML) $(OUT_JS) $(OUT_WASM) $(OUT_DATA)

SRC        := chess_viewer.c pgn_loader.c
ASSETS_DIR := assets
SHELL_FILE := shell.html
VIDEO_EXPORT := video_export.js

DEST ?= /mnt/c/Users/$(USER)/Desktop/chess_wasm

CFLAGS  := -Os -Wall -DPLATFORM_WEB -I. -I$(RAYLIB_SRC)
LDFLAGS := ./$(RAYLIB_LIB) \
           -s USE_GLFW=3 \
           -s ASYNCIFY \
           -s TOTAL_MEMORY=67108864 \
           -s FORCE_FILESYSTEM=1 \
           --preload-file $(ASSETS_DIR) \
           --shell-file $(SHELL_FILE)

# === Utility Functions ===
define WITH_EMSDK
bash -c 'source "$(EMSDK_ENV)" >/dev/null 2>&1 && $(1)'
endef

# === Commands ===
CLONE_EMSDK   = test -d "$(EMSDK_DIR)" || git clone --depth 1 https://github.com/emscripten-core/emsdk.git "$(EMSDK_DIR)"
INSTALL_EMSDK = test -x "$(EMSDK_EMCC)" || (cd "$(EMSDK_DIR)" && ./emsdk install latest && ./emsdk activate latest)
CLONE_RAYLIB  = test -d "$(RAYLIB_SRC)" || git clone --depth 1 https://github.com/raysan5/raylib.git "$(RAYLIB_REPO_DIR)"
COPY_RAYLIB   = cp -f "$(RAYLIB_LIB_SRC)" "./$(RAYLIB_LIB)"
CHECK_ASSETS  = test -f "$(ASSETS_DIR)/chess_board.glb" || (echo "Falta $(ASSETS_DIR)/chess_board.glb"; exit 1)
PATCH_JPG     = sed -i 's/SUPPORT_FILEFORMAT_JPG[[:space:]]\+0/SUPPORT_FILEFORMAT_JPG        1/' "$(RAYLIB_CONFIG)"

RM_OUT        = rm -f $(OUTPUTS)
RM_LIB        = rm -f "./$(RAYLIB_LIB)"
RM_RAYLIB_DIR = rm -rf "$(RAYLIB_REPO_DIR)"
RM_EMSDK_DIR  = rm -rf "$(EMSDK_DIR)"
RM_FFMPEG_DIR = rm -rf "$(FFMPEG_DIR)"

.PHONY: all emsdk raylib ffmpeg run copy clean fclean emsdk-clean ffmpeg-clean help

# ============================================================
# TARGETS
# ============================================================

help:
	@echo ""
	@echo "📦 Chess Viewer WASM - Makefile"
	@echo "================================"
	@echo ""
	@echo "Targets principales:"
	@echo "  make all          - Compilar todo (emsdk, raylib, ffmpeg, chess)"
	@echo "  make run          - Compilar y servir en http://localhost:8000"
	@echo "  make copy         - Copiar archivos finales a DEST"
	@echo ""
	@echo "Targets individuales:"
	@echo "  make emsdk        - Descargar e instalar emscripten"
	@echo "  make raylib       - Compilar raylib para WASM"
	@echo "  make ffmpeg       - Descargar FFmpeg.wasm precompilado"
	@echo ""
	@echo "Limpieza:"
	@echo "  make clean        - Borrar outputs compilados"
	@echo "  make fclean       - clean + raylib (pero mantiene emsdk)"
	@echo "  make ffmpeg-clean - Borrar carpeta ffmpeg/"
	@echo "  make emsdk-clean  - Borrar emsdk completamente"
	@echo ""
	@echo "Variables:"
	@echo "  EMSDK_DIR=path    - Ubicación de emsdk"
	@echo "  RAYLIB_SRC=path   - Ubicación de raylib/src"
	@echo "  DEST=path         - Destino para 'make copy'"
	@echo ""

all: $(OUT_HTML)

# ============================================================
# EMSCRIPTEN SDK
# ============================================================

emsdk:
	@echo "[emsdk] Clonando si es necesario..."
	@$(CLONE_EMSDK)
	@echo "[emsdk] Instalando si es necesario..."
	@$(INSTALL_EMSDK)
	@echo "✓ emsdk listo en $(EMSDK_DIR)"

# ============================================================
# RAYLIB
# ============================================================

raylib: emsdk
	@echo "[raylib] Clonando si es necesario..."
	@$(CLONE_RAYLIB)
	@echo "[raylib] Verificando parche JPG en config.h..."
	@grep -Eq 'SUPPORT_FILEFORMAT_JPG[[:space:]]+1' "$(RAYLIB_CONFIG)" || { \
		echo "[raylib] Parcheando para soporte JPG..."; \
		$(PATCH_JPG); \
		rm -f "$(RAYLIB_LIB_SRC)" "./$(RAYLIB_LIB)"; \
	}
	@echo "[raylib] Compilando libraylib.a para PLATFORM_WEB..."
	@test -f "$(RAYLIB_LIB_SRC)" || $(call WITH_EMSDK,emmake $(MAKE) -C "$(RAYLIB_SRC)" PLATFORM=PLATFORM_WEB -j4)
	@$(COPY_RAYLIB)
	@echo "✓ libraylib.a listo (PLATFORM_WEB, JPG habilitado)"

# ============================================================
# FFMPEG.WASM (corregido)
# ============================================================

define DOWNLOAD_WRAPPER
	@echo "  ↓ Descargando $(1) (wrapper)..." && \
	mkdir -p $(FFMPEG_DIR) && \
	curl -fsSL "$(FFMPEG_WRAPPER_URL)/$(1)" -o "$(FFMPEG_DIR)/$(1)" || \
	(echo "  ⚠ Fallback a GitHub release..." && \
	curl -fsSL "https://github.com/ffmpegwasm/ffmpeg.wasm/releases/download/v$(FFMPEG_VERSION)/$(1)" -o "$(FFMPEG_DIR)/$(1)")
endef

define DOWNLOAD_CORE
	@echo "  ↓ Descargando $(1) (core)..." && \
	mkdir -p $(FFMPEG_DIR) && \
	curl -fsSL "$(FFMPEG_CORE_URL)/$(1)" -o "$(FFMPEG_DIR)/$(1)" || \
	(echo "  ⚠ Fallback a GitHub release..." && \
	curl -fsSL "https://github.com/ffmpegwasm/ffmpeg.wasm/releases/download/v$(FFMPEG_VERSION)/$(1)" -o "$(FFMPEG_DIR)/$(1)")
endef

$(FFMPEG_DIR)/ffmpeg.min.js:
	$(call DOWNLOAD_WRAPPER,ffmpeg.min.js)

$(FFMPEG_DIR)/ffmpeg.js:
	$(call DOWNLOAD_WRAPPER,ffmpeg.js)

$(FFMPEG_DIR)/ffmpeg-core.js:
	$(call DOWNLOAD_CORE,ffmpeg-core.js)

$(FFMPEG_DIR)/ffmpeg-core.wasm:
	$(call DOWNLOAD_CORE,ffmpeg-core.wasm)

ffmpeg: $(FFMPEG_TARGETS)
	@echo "✓ FFmpeg.wasm v$(FFMPEG_VERSION) listo en $(FFMPEG_DIR)/"
	@ls -lh $(FFMPEG_DIR)/ | grep -E "\.(js|wasm)$$" | awk '{print "  ✓ " $$9 " (" $$5 ")"}'

# ============================================================
# ASSETS CHECK
# ============================================================

check: raylib ffmpeg
	@echo "[check] Validando assets..."
	@$(CHECK_ASSETS)

# ============================================================
# CHESS VIEWER COMPILATION
# ============================================================

$(OUT_HTML): check $(SRC) pgn_loader.h $(SHELL_FILE)
	@echo "[build] Compilando Chess Viewer WASM..."
	$(call WITH_EMSDK,$(EMCC) $(SRC) -o $(OUT_HTML) $(CFLAGS) $(LDFLAGS))
	@echo ""
	@echo "✓ Build completado exitosamente!"
	@echo ""
	@echo "Archivos generados:"
	@ls -lh $(OUTPUTS) $(VIDEO_EXPORT) | awk '{print "  " $$9 " (" $$5 ")"}'
	@echo ""
	@echo "Próximo paso:"
	@echo "  make run    # Inicia servidor en http://localhost:8000"
	@echo ""

# ============================================================
# RUN & DEPLOY
# ============================================================

run: all
	@echo ""
	@echo "🚀 Iniciando servidor HTTP en puerto 8000..."
	@echo "   Abre: http://localhost:8000"
	@echo ""
	python3 -m http.server 8000

copy: all
	@echo "[copy] Copiando archivos a $(DEST)..."
	@mkdir -p "$(DEST)"
	@cp $(OUTPUTS) $(VIDEO_EXPORT) "$(DEST)/"
	@echo "✓ Archivos copiados:"
	@ls -lh "$(DEST)" | tail -n +2 | awk '{print "  " $$9 " (" $$5 ")"}'

# ============================================================
# CLEANUP
# ============================================================

clean:
	@echo "[clean] Borrando outputs compilados..."
	$(RM_OUT)
	@echo "✓ Limpiado"

fclean: clean
	@echo "[fclean] Borrando raylib..."
	$(RM_LIB)
	$(RM_RAYLIB_DIR)
	@echo "✓ Raylib limpiado (mantén emsdk)"

ffmpeg-clean:
	@echo "[ffmpeg-clean] Borrando $(FFMPEG_DIR)/..."
	$(RM_FFMPEG_DIR)
	@echo "✓ FFmpeg limpiado"

emsdk-clean:
	@echo "[emsdk-clean] Borrando emsdk completamente..."
	$(RM_EMSDK_DIR)
	@echo "✓ emsdk limpiado"

.DEFAULT_GOAL := help