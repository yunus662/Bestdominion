# Conqueror Engine

Conqueror Engine is a high-performance, modular nation-building game engine written in C++ and compiled to WebAssembly using Emscripten. It features multiple subsystems—including unit simulation with A* pathfinding, combat simulation, economic modeling, government policy management, real-time text chat, and map integration via Leaflet—all integrated into a single browser-based application.

## Features

- **Modular Architecture:**  
  The engine is divided into clearly separated modules (units, combat, economy, government, chat, etc.), each following a standard initialization, update, and shutdown lifecycle.

- **A* Pathfinding:**  
  Units use a real A* algorithm to compute efficient paths on a grid-based map with obstacles.

- **Network and Multiplayer Ready:**  
  Integration with Socket.IO enables realtime lobby and notification handling, with clear placeholders for extending to full multiplayer functionality.

- **Resource Loading:**  
  External files (assets, configuration files, etc.) are loaded via a built-in ResourceLoader, making it easy to manage game content.

- **WebAssembly Integration:**  
  The core engine (and its additional gameplay modules) is compiled to WASM for high performance in modern browsers.

- **Map Integration:**  
  Interactive maps are displayed using Leaflet, falling back to a basic map setup if needed.

- **Responsive UI:**  
  A professionally designed loading screen, notifications, and overall UI styling ensure a smooth user experience.

## Project Structure

- **index.html**  
  The single HTML entry point that loads the WASM modules, sets up external libraries (Leaflet, Socket.IO), displays the loading screen, and integrates the game UI.

- **game_engine.cpp**  
  The core engine module written in C++—implements unit simulation, A* pathfinding, and additional engine subsystems.

- **gameplay_stitched.cpp**  
  An aggregate module that integrates additional gameplay systems and assets.

- **engine-import.js / game-loader.js**  
  JavaScript modules that import and initialize engine components and external libraries. (Recent versions may merge these or be split by functionality.)

- **config.json**  
  A configuration file containing settings for the title, default lobby, gameplay options, map configurations, network parameters, and a list of resource files.

- **style.css**  
  The main stylesheet for the project, providing professional, responsive styling for the loading screen, game root, map container, notifications, and other UI components.

- **libs/**  
  Contains third-party libraries (such as Leaflet, and optionally additional helper libraries).

- **assets/**  
  The folder for external resource files that are preloaded into the Emscripten virtual filesystem.

## Build and Deployment

### Building With Emscripten

To compile the core engine module (game_engine.cpp) to WebAssembly, run:

```bash
emcc game_engine.cpp -O2 -s WASM=1 -s USE_PTHREADS=1 \
   -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap','FS']" \
   --preload-file assets -o game_engine.html
