# Troubleshooting Conqueror Engine (Revised)

This document outlines common issues you may encounter while working with the Conqueror Engine (Revised) project and provides guidance for troubleshooting and resolving them.

---

## 1. WASM Module Loading Failures

**Symptom:**  
- Console errors such as "Failed to load game_engine.wasm" or "gameplay_stitched.wasm aborted!"

**Possible Causes:**  
- Incorrect file paths or missing WASM modules in the deployment directory.  
- The server not serving WASM files with the proper MIME type (`application/wasm`).  
- Network connectivity issues.

**Solutions:**  
- Verify that `game_engine.wasm` and `gameplay_stitched.wasm` are located in the correct directory (typically alongside `index.html`).  
- Ensure your server or Docker configuration serves `.wasm` files with the proper MIME type.  
- Check your browser’s network logs for additional details and verify network connectivity.

---

## 2. Socket.IO Connection Issues

**Symptom:**  
- No connection to the Socket.IO server or connection-related errors (e.g., missing lobby code, connection timeouts).

**Possible Causes:**  
- An incorrect or unreachable Socket.IO endpoint URL.  
- Network firewall or CORS restrictions.

**Solutions:**  
- Confirm that the Socket.IO endpoint URL specified in **engine-import.js** is correct and accessible.  
- Check the browser console for CORS or connection errors.  
- If no lobby code is provided in the URL, ensure that the default ("ROOM-0") is correctly set.

---

## 3. Leaflet Map Initialization Problems

**Symptom:**  
- The fallback Leaflet map does not initialize or display correctly.

**Possible Causes:**  
- Incorrect import of the Leaflet library or missing Leaflet CSS.  
- The `<div id="map"></div>` element is missing, has insufficient dimensions, or is being hidden erroneously.

**Solutions:**  
- Verify that the Leaflet library is properly imported from `./libs/leaflet-src.esm.js` and that the Leaflet CSS is loaded (via your CDN link in **index.html**).  
- Ensure the HTML contains the `<div id="map"></div>` container and that its dimensions are defined (as specified in **style.css**).  
- Use browser developer tools to inspect errors related to Leaflet.

---

## 4. UI Rendering Issues

**Symptom:**  
- The loading screen remains visible or the game container doesn’t appear, even after initialization.

**Possible Causes:**  
- Errors during WASM module instantiation or execution that block the UI transition.  
- JavaScript errors in the initialization logic within **engine-import.js** or **game-loader.js**.

**Solutions:**  
- Check the browser console for any error messages during initialization.  
- Verify that after successful WASM module loading, the script hides the loading screen (`#loading-screen`) and displays the game container (`#game-root`).  
- Ensure that the CSS (from **style.css**) is properly applied.

---

## 5. General Debugging Tips

- **Browser Developer Tools:**  
  Use the console and network tabs to inspect error messages and verify resource loading.
  
- **Local Server:**  
  Ensure you run a local server (using **run.sh** or similar) rather than opening the HTML file directly in the browser.
  
- **Dependency Checks:**  
  Confirm that all external libraries (Emscripten, Socket.IO, Leaflet) are up-to-date and compatible.
  
- **Log Details:**  
  Review both server and client logs to help pinpoint issues.

---

## 6. Reporting Issues

If you continue to experience problems:
- Refer to the [CONTRIBUTING.md](CONTRIBUTING.md) file for guidelines on reporting issues.
- Provide detailed error logs and describe the steps necessary to reproduce the problem.

Thank you for using Conqueror Engine (Revised). Your input is invaluable in helping us improve this project.
