#!/bin/bash
# build.sh - Build script for Conqueror Engine WebAssembly modules

# Exit immediately if a command exits with a non-zero status
set -e

# Create the output directory if it does not exist
OUTPUT_DIR="dist"
mkdir -p ${OUTPUT_DIR}

echo "Building game_engine.cpp..."
emcc game_engine.cpp -O2 \
  -s WASM=1 \
  -s USE_PTHREADS=1 \
  -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','FS']" \
  --preload-file assets \
  -o ${OUTPUT_DIR}/game_engine.html

echo "Building gameplay_stitched.cpp..."
emcc gameplay_stitched.cpp -O2 \
  -s WASM=1 \
  -s USE_PTHREADS=1 \
  -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','FS']" \
  --preload-file assets \
  -o ${OUTPUT_DIR}/gameplay_stitched.html

echo "Build complete. Output files are in the '${OUTPUT_DIR}' directory."
