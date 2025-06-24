/*
 * engine-import.js
 *
 * This module initializes the Conqueror Engine by:
 * - Setting up URL parameters and lobby code.
 * - Establishing a Socket.IO connection.
 * - Loading WebAssembly modules for the game engine.
 * - Falling back to a basic Leaflet map if needed.
 * - Transitioning from the loading screen to the main game UI.
 */

import * as L from "./libs/leaflet-src.esm.js";

export async function initializeGame() {
  console.log("engine-import.js: Initializing Conqueror Engine...");

  // Set up lobby from URL parameters; default to "ROOM-0" for solo mode.
  const params = new URLSearchParams(window.location.search);
  const lobby = params.get("lobby") || "ROOM-0";
  if (!params.get("lobby")) {
    console.warn("No lobby code provided. Defaulting to ROOM-0 (Solo Mode).");
    const soloWarning = document.createElement("div");
    soloWarning.id = "solo-warning";
    soloWarning.textContent = "⚠ No lobby code provided. Running in SOLO mode.";
    document.body.appendChild(soloWarning);
  }
  window.lobbyCode = lobby;

  // Establish Socket.IO connection.
  // Assumes that the Socket.IO script is loaded globally via CDN and available as 'io'.
  const socket = io("https://3db21b63-ee56-43d9-877f-e5b68b462df6-00-3s3v6c139aq4b.spock.repl.co/");
  socket.on("connect", () => {
    console.log(`Socket.IO: Connected as ${socket.id}`);
    socket.emit("join-lobby", lobby);
  });
  socket.on("map-update", (data) => {
    console.log("Map update received:", data);
    if (typeof window.handleMapUpdate === "function") {
      window.handleMapUpdate(data);
    }
  });
  window.socket = socket;

  // Function: Initialize a fallback map using Leaflet.
  function initMapFallback() {
    console.log("Initializing fallback Leaflet map.");
    const map = L.map("map").setView([51.505, -0.09], 13);
    L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
      maxZoom: 19,
      attribution: "© OpenStreetMap"
    }).addTo(map);
    window.leafletMap = map;
  }

  // Concurrently load the WASM modules.
  try {
    const [engineModule, stitchedModule] = await Promise.all([
      fetch("./game_engine.wasm").then(response => {
        if (!response.ok) {
          throw new Error(`Failed to load game_engine.wasm: ${response.statusText}`);
        }
        return WebAssembly.instantiateStreaming(response, {
          env: { abort: () => console.error("game_engine.wasm aborted!") }
        });
      }),
      fetch("./gameplay_stitched.wasm").then(response => {
        if (!response.ok) {
          throw new Error(`Failed to load gameplay_stitched.wasm: ${response.statusText}`);
        }
        return WebAssembly.instantiateStreaming(response, {
          env: { abort: () => console.error("gameplay_stitched.wasm aborted!") }
        });
      })
    ]);
    console.log("WASM modules loaded successfully.");

    // Optionally call main() from each WASM module if exported.
    if (engineModule.instance.exports.main) {
      console.log("Calling main() from game_engine.wasm...");
      engineModule.instance.exports.main();
    } else {
      console.warn("No main() export found in game_engine.wasm.");
    }
    if (stitchedModule.instance.exports.main) {
      console.log("Calling main() from gameplay_stitched.wasm...");
      stitchedModule.instance.exports.main();
    } else {
      console.warn("No main() export found in gameplay_stitched.wasm.");
    }
  } catch (error) {
    console.error("Error loading WASM modules:", error);
    const loadingScreen = document.getElementById("loading-screen");
    if (loadingScreen) {
      loadingScreen.innerHTML =
        "<h1>Error loading game!</h1><p>Please check the console.</p>";
    }
    // Initialize fallback map even if WASM modules fail to load.
    initMapFallback();
    return;
  }

  // Transition the UI: hide the loading screen and show the main game container.
  document.getElementById("loading-screen").style.display = "none";
  document.getElementById("game-root").style.display = "block";

  // Initialize fallback Leaflet map if it hasn't been set.
  if (!window.leafletMap) {
    initMapFallback();
  }

  console.log("Conqueror Engine initialization complete.");
  return { socket, lobby };
}

// Optionally, to auto-start initialization when this module is imported,
// uncomment the following lines:
// initializeGame().catch(error => {
//   console.error("Engine initialization error:", error);
// });
