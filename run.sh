#!/bin/bash
# run.sh - Start a local development server for Conqueror Engine

# Set the port for the local server (default is 8080)
PORT=8080

# Ensure we're serving the 'dist' directory (adjust if necessary)
OUTPUT_DIR="dist"

if [ ! -d "$OUTPUT_DIR" ]; then
  echo "Error: Directory '$OUTPUT_DIR' does not exist. Please build the project first."
  exit 1
fi

echo "Serving ${OUTPUT_DIR} on http://localhost:${PORT} ..."
cd "$OUTPUT_DIR"

# Start a Python 3 HTTP server
python3 -m http.server ${PORT}
