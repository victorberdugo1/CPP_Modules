# Makefile - build WASM (emscripten + raylib) para el chess viewer
# Pensado para correr dentro de WSL (Ubuntu) en Windows.
#
# "make" a secas se basta solo: si falta emsdk lo clona e instala, si falta
# raylib (compilado para PLATFORM_WEB) lo clona y compila, y luego compila
# el visor. No hace falta sourcear nada a mano.
#
#   make                                            # bootstrap completo + build
#   make EMSDK_DIR=/ruta/a/emsdk                    # usa/crea ese emsdk
#   make RAYLIB_SRC=/ruta/a/raylib/src              # usa/crea ese raylib
#   make run                                        # sirve con python http.server en :8000
#   make copy DEST=/mnt/c/Users/tuuser/Desktop/chess_wasm
#   make clean            # borra el build wasm
#   make fclean           # + libraylib.a local + raylib/ clonado
#   make emsdk-clean      # borra tambien el emsdk clonado (pesado, no se borra con fclean)
#
# Requisito: conexion a internet la primera vez (emsdk baja node/clang de
# storage.googleapis.com, ~terminado el `emsdk install`).

EMSDK_DIR  ?= emsdk
EMSDK_ENV  := $(EMSDK_DIR)/emsdk_env.sh
EMSDK_EMCC := $(EMSDK_DIR)/upstream/emscripten/emcc

# Con PLATFORM=PLATFORM_WEB, el make de raylib genera "libraylib.web.a"
# (no "libraylib.a"), asi que apuntamos directo a ese nombre.
RAYLIB_SRC      ?= raylib/src
RAYLIB_REPO_DIR := $(patsubst %/src,%,$(RAYLIB_SRC))
RAYLIB_LIB_SRC  := $(RAYLIB_SRC)/libraylib.web.a
RAYLIB_LIB      := libraylib.a
RAYLIB_CONFIG   := $(RAYLIB_SRC)/config.h

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

# Carpeta destino de "make copy" (cambiala a la tuya o pasala con DEST=...)
DEST ?= /mnt/c/Users/$(USER)/Desktop/chess_wasm

CFLAGS  := -Os -Wall -DPLATFORM_WEB -I. -I$(RAYLIB_SRC)
LDFLAGS := ./$(RAYLIB_LIB) \
           -s USE_GLFW=3 \
           -s ASYNCIFY \
           -s TOTAL_MEMORY=67108864 \
           -s FORCE_FILESYSTEM=1 \
           --preload-file $(ASSETS_DIR) \
           --shell-file $(SHELL_FILE)

# Ejecuta $(1) en una shell con el entorno de emsdk ya cargado (PATH a
# emcc/emmake/clang/node, EM_CONFIG, etc). Se usa bash -c con una sola
# invocacion para que el "source" y el comando corran en el mismo proceso.
define WITH_EMSDK
bash -c 'source "$(EMSDK_ENV)" >/dev/null 2>&1 && $(1)'
endef

# --- comandos nombrados ---
CLONE_EMSDK   = test -d "$(EMSDK_DIR)" || git clone --depth 1 https://github.com/emscripten-core/emsdk.git "$(EMSDK_DIR)"
INSTALL_EMSDK = test -x "$(EMSDK_EMCC)" || (cd "$(EMSDK_DIR)" && ./emsdk install latest && ./emsdk activate latest)
CLONE_RAYLIB  = test -d "$(RAYLIB_SRC)" || git clone --depth 1 https://github.com/raysan5/raylib.git "$(RAYLIB_REPO_DIR)"
COPY_RAYLIB   = cp -f "$(RAYLIB_LIB_SRC)" "./$(RAYLIB_LIB)"
CHECK_ASSETS  = test -f "$(ASSETS_DIR)/chess_board.glb" || (echo "Falta $(ASSETS_DIR)/chess_board.glb"; exit 1)
# Parchea config.h: activa SUPPORT_FILEFORMAT_JPG (viene en 0 por defecto).
PATCH_JPG     = sed -i 's/SUPPORT_FILEFORMAT_JPG[[:space:]]\+0/SUPPORT_FILEFORMAT_JPG        1/' "$(RAYLIB_CONFIG)"
RM_OUT        = rm -f $(OUTPUTS)
RM_LIB        = rm -f "./$(RAYLIB_LIB)"
RM_RAYLIB_DIR = rm -rf "$(RAYLIB_REPO_DIR)"
RM_EMSDK_DIR  = rm -rf "$(EMSDK_DIR)"

.PHONY: all emsdk raylib run copy clean fclean emsdk-clean check

all: $(OUT_HTML)

# Clona e instala emsdk si falta (no requiere haberlo sourceado antes).
emsdk:
	@$(CLONE_EMSDK)
	@$(INSTALL_EMSDK)
	@echo "emsdk listo en $(EMSDK_DIR)"

# Clona raylib si falta y lo compila para PLATFORM_WEB si falta el .a,
# usando el emsdk local; copia libraylib.a a la raiz del proyecto.
raylib: emsdk
	@$(CLONE_RAYLIB)
	@grep -Eq 'SUPPORT_FILEFORMAT_JPG[[:space:]]+1' "$(RAYLIB_CONFIG)" || { \
		echo "Parcheando $(RAYLIB_CONFIG) para soporte JPG..."; \
		$(PATCH_JPG); \
		rm -f "$(RAYLIB_LIB_SRC)" "./$(RAYLIB_LIB)"; \
	}
	@test -f "$(RAYLIB_LIB_SRC)" || $(call WITH_EMSDK,emmake $(MAKE) -C "$(RAYLIB_SRC)" PLATFORM=PLATFORM_WEB -j4)
	@$(COPY_RAYLIB)
	@echo "libraylib.a listo (PLATFORM_WEB, JPG habilitado) en ./$(RAYLIB_LIB)"

check: raylib
	@$(CHECK_ASSETS)

$(OUT_HTML): check $(SRC) pgn_loader.h $(SHELL_FILE)
	$(call WITH_EMSDK,$(EMCC) $(SRC) -o $(OUT_HTML) $(CFLAGS) $(LDFLAGS))
	@echo "Build OK -> $(OUTPUTS)"

run: all
	python3 -m http.server 8000

copy: all
	mkdir -p "$(DEST)"
	cp $(OUTPUTS) $(VIDEO_EXPORT) "$(DEST)/"
	@echo "Copiado a $(DEST)"

clean:
	$(RM_OUT)

fclean: clean
	$(RM_LIB)
	$(RM_RAYLIB_DIR)

emsdk-clean:
	$(RM_EMSDK_DIR)