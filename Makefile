# Makefile for Conqueror Engine Project

# Compiler (assumes emcc is on your PATH)
CXX = emcc

# Common compiler flags:
# -O2: Optimization level 2.
# -s WASM=1: Compile to WebAssembly.
# -s USE_PTHREADS=1: Enable multi-threading (if supported).
# -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','FS']": Expose runtime methods needed for integration.
# --preload-file assets: Preload the entire assets folder.
CFLAGS = -O2 -s WASM=1 -s USE_PTHREADS=1 -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','FS']" --preload-file assets

# Targets:
TARGET_ENGINE = game_engine.html
TARGET_STITCHED = gameplay_stitched.html

all: $(TARGET_ENGINE) $(TARGET_STITCHED)

$(TARGET_ENGINE): game_engine.cpp
    $(CXX) game_engine.cpp $(CFLAGS) -o $(TARGET_ENGINE)

$(TARGET_STITCHED): gameplay_stitched.cpp
    $(CXX) gameplay_stitched.cpp $(CFLAGS) -o $(TARGET_STITCHED)

clean:
    rm -rf $(TARGET_ENGINE) $(TARGET_STITCHED) *.js *.wasm *.data

.PHONY: all clean
