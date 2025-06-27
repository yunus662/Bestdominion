// game-loader.js
import { initializeGame } from "./engine-import.js";

console.log("game-loader.js: Starting engine initialization...");

initializeGame()
  .then(({ socket, lobby } = {}) => {
    console.log("game-loader.js: Engine initialized successfully.", { lobby, socketId: socket ? socket.id : "N/A" });
  })
  .catch(err => {
    console.error("game-loader.js: Engine initialization failed:", err);
  });
