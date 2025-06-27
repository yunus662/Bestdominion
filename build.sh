#!/bin/bash
# build.sh - Build script for Conqueror Engine WebAssembly modules

set -euo pipefail

# Constants
OUTPUT_DIR="dist"
SRC_FILES=(
  "game_engine.cpp"
  "gameplay_stitched.cpp"
)

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Build loop
for src_file in "${SRC_FILES[@]}"; do
  base_name=$(basename "$src_file" .cpp)
  echo "Building $src_file..."
  emcc "$src_file" -O2 \
    -s WASM=1 \
    -s USE_PTHREADS=1 \
    -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','FS']" \
    --preload-file assets \
    -o "$OUTPUT_DIR/${base_name}.html"
done

echo "Build complete. Output files are in '$OUTPUT_DIR'"
