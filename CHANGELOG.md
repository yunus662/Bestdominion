# Changelog

All notable changes to the Conqueror Engine project will be documented in this file.

## [1.0.0] - YYYY-MM-DD (game not meant to be released yet)
### Added
- Initial release of the Conqueror Engine.
- Implemented modular engine architecture with the following modules:
  - **UnitModule** with A* pathfinding and unit simulation.
  - **EconomyModule** for economic simulation.
  - **GovernmentModule** for political decision-making.
  - **ChatModule** for in-browser text chat.
  - **MapModule** integrated via Leaflet for map display.
  - **ResourceLoaderModule** to pre-load external asset files.
- A fully functional **index.html** that loads two separate WebAssembly modules (from `game_engine.cpp` and `gameplay_stitched.cpp`).
- A production‑ready **engine-import.js** (or minimal **game-loader.js**) for additional initialization.
- Comprehensive styling in **style.css**.
- Build instructions and project documentation in **README.md**.
- This initial version has been built and deployed using Emscripten.

## [Unreleased]
### Changed
- Future planned updates and improvements will be outlined here.
