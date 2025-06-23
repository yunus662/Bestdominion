#!/bin/bash
# fix_wasm.sh
# This script checks if game_engine.wasm exists in the repository root.
# If not found, it searches for it and copies it to the root directory.

echo "Checking for game_engine.wasm in the repository root..."

# Check if game_engine.wasm exists in the repository root.
if [ ! -f "./game_engine.wasm" ]; then
    echo "game_engine.wasm not found in the repository root."
    echo "Searching for game_engine.wasm in the repository..."
    
    # Find the file – adjust the search path ('.') if needed.
    WASM_PATH=$(find . -type f -name "game_engine.wasm" | head -n 1)
    
    if [ -z "$WASM_PATH" ]; then
        echo "Error: game_engine.wasm not found anywhere in the repository."
        echo "Please ensure it has been built and is placed correctly."
        exit 1
    else
        echo "Found game_engine.wasm at: $WASM_PATH"
        echo "Copying it to the repository root..."
        cp "$WASM_PATH" "./game_engine.wasm"
        if [ $? -eq 0 ]; then
            echo "File successfully copied to the repository root."
        else
            echo "Error: Unable to copy the file. Please check your permissions."
            exit 1
        fi
    fi
else
    echo "game_engine.wasm already exists in the repository root. No action required."
fi

echo "Done."
