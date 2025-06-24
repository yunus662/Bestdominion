// game-loader.js - Additional integration for the Conqueror Engine

console.log("Game Loader: Initializing...");

document.addEventListener("DOMContentLoaded", () => {
  console.log("Game Loader: DOM fully loaded.");

  // Check if the Emscripten Module is available.
  if (typeof Module !== "undefined") {
    console.log("Game Loader: Emscripten Module detected.");
  } else {
    console.warn("Game Loader: Emscripten Module not found.");
  }

  // Setup UI notifications using the Socket.IO connection.
  const notificationElement = document.getElementById("notification");
  if (window.socket) {
    window.socket.on("notification", (data) => {
      console.log("Game Loader: Notification received:", data);
      if (notificationElement && data.message) {
        notificationElement.textContent = data.message;
        // Clear the notification after 3 seconds.
        setTimeout(() => {
          notificationElement.textContent = "";
        }, 3000);
      }
    });
  } else {
    console.warn("Game Loader: Socket.IO object not detected.");
  }
  
  // Load additional configuration from an external config.json file.
  fetch("config.json")
    .then(response => {
      if (!response.ok) {
        throw new Error("Failed to load config.json");
      }
      return response.json();
    })
    .then(config => {
      console.log("Game Loader: Configuration loaded", config);
      // Example: Update the document title based on the configuration.
      if (config.title) {
        document.title = config.title;
      }
      // Optional: Apply other configuration settings as needed.
    })
    .catch(err => {
      console.error("Game Loader: Error loading config.json:", err);
    });

  // Additional integration points:
  // - You can poll the game engine's status via Module.ccall if needed.
  // - Attach additional event listeners, or initialize extra UI elements.
  
  console.log("Game Loader: Initialization complete.");
});
