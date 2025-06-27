# Conqueror Engine (Revised) Architecture

This document describes the architecture of the Conqueror Engine (Revised) project, detailing its core components, module interactions, and overall design principles. It serves as a guide for developers to understand, maintain, and extend the project.

## 1. Overview

Conqueror Engine (Revised) is a high-performance, modular game engine focused on nation-building simulation in a browser environment. It combines C++ game logic compiled to WebAssembly (WASM) with modern JavaScript front-end components, enabling robust performance and smooth interactivity.

## 2. Core Components

### 2.1 WebAssembly Modules
- **`game_engine.cpp`:**  
  Contains the core game logic, including unit simulation, A* pathfinding, and structural subsystems. It is compiled to a WASM module (`game_engine.wasm`) using Emscripten.
  
- **`gameplay_stitched.cpp`:**  
  Integrates additional gameplay features and assets, compiled into its own WASM module (`gameplay_stitched.wasm`). This module allows separation of gameplay logic from core engine functionality.

### 2.2 JavaScript Integration
- **`engine-import.js`:**  
  An ES module that orchestrates the initialization process. Responsibilities include:
  - Extracting URL parameters (lobby codes and configuration).
  - Establishing a Socket.IO connection for real-time interactions.
  - Concurrently loading WASM modules and invoking their entry points.
  - Managing fallback logic for Leaflet-based map rendering.
  
- **`game-loader.js`:**  
  A lightweight loader that imports and invokes the initialization function from `engine-import.js`. It logs essential runtime information, providing a clear startup sequence.

### 2.3 Front-End and UI Components
- **`index.html`:**  
  The primary entry point for the browser. It:
  - Integrates external libraries (Leaflet CSS/JS, Socket.IO).
  - Provides a professional loading screen that transitions to the game view once initialization is complete.
  - Embeds the main script that triggers engine initialization.
  
- **`style.css`:**  
  Contains styling rules for the overall appearance of the application, including the loading screen, notifications, and responsive design adjustments.

### 2.4 Server and Deployment
- **Docker and Nginx:**  
  A **Dockerfile** is provided to build a container with Nginx serving the built assets from the `dist/` directory. Custom configurations like **nginx.conf** ensure proper MIME types and caching policies are applied.
  
- **Build and Execution Scripts:**  
  - **`build.sh`:** A shell script to compile both WASM modules with Emscripten and place outputs in `dist/`.
  - **`run.sh`:** Launches a local development server to serve built files.
  - **Terminal and Makefile commands:** Documented collectively in **TERMINAL_COMMANDS.md** for reproducible workflows.

## 3. External Dependencies

- **Emscripten:**  
  Transforms C++ source code into WebAssembly, exposing specific runtime methods required for module interaction.
  
- **Leaflet:**  
  Provides interactive mapping capabilities. A fallback initialization is available if the main engine does not instantiate a map.
  
- **Socket.IO:**  
  Enables real‑time communication between clients and the server, managing lobby functions and dynamic map updates.
  
- **Socket.IO CDN and Additional Libraries:**  
  CDN-hosted external scripts support rapid runtime integration. Google Fonts and custom styling ensure visual consistency.

## 4. Module Interactions

The architecture is designed to separate concerns:
- **C++ WASM Modules** handle computationally intensive logic, ensuring performance.
- **JavaScript Modules** act as glue, handling UI rendering, network communication, and error handling.
- **HTML & CSS** manage user experience and view transitions, ensuring the application remains responsive and robust on different devices.

## 5. Build and Deployment Workflow

1. **Local Development:**
   - Use the provided **Makefile** or **build.sh** to compile and generate WebAssembly modules.
   - Launch the application locally with **run.sh** to verify functionality.
   
2. **Continuous Integration:**
   - GitHub Actions (configured in `.github/workflows/build.yml`) validates builds and artifacts on every commit to the `revised` branch.
   
3. **Containerized Deployment:**
   - Build a Docker image using the provided **Dockerfile**.
   - Deploy the container, ensuring the server is set to serve static files from the `dist/` directory with proper caching and MIME type handling.

## 6. Future Enhancements and Roadmap
- **Modular Expansion:**  
  New gameplay features can be developed as separate modules and integrated without affecting core engine stability.
  
- **Performance Optimization:**  
  Continuous profiling of WASM modules and fine-tuning of JavaScript interactions for lower latency.
  
- **Enhanced Real-Time Features:**  
  Improved networking capabilities and more dynamic map updates leveraging the Socket.IO ecosystem.

---

This document should serve as a detailed guide for understanding the inner workings of Conqueror Engine (Revised). It enables contributors to quickly grasp the project structure and highlights the path for future developments.

For any questions or further clarifications, please consult the [CONTRIBUTING.md](CONTRIBUTING.md) file or reach out to the maintainers directly.
