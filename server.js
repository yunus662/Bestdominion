// server.js
// Production‑Quality Game Server for Conqueror Engine (Replit Version)
//
// This server uses Express to serve static files and an HTTP endpoint for your main game page,
// while Socket.IO handles real‑time lobby and map communications. The module leverages ES module syntax,
// normalizes __dirname for Replit’s environment, and includes enhanced error handling and logging.

import express from "express";
import http from "http";
import { Server as SocketIO } from "socket.io";
import path from "path";
import { fileURLToPath } from "url";

// Normalize __dirname for ES Modules on Replit
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// Create the Express app and HTTP server
const app = express();
const server = http.createServer(app);

// Attach Socket.IO to the server and allow CORS from any origin (adjust in production)
const io = new SocketIO(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"]
  }
});

// Serve static files from the current directory (e.g., HTML, CSS, JS assets)
app.use(express.static(path.join(__dirname)));

// Main route to serve the index.html
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "index.html"));
});

// Socket.IO event handlers for lobby and map communications
io.on("connection", (socket) => {
  console.log(`🛰️ Client connected: ${socket.id}`);

  // Join a lobby room
  socket.on("join-lobby", (lobby) => {
    try {
      socket.join(lobby);
      console.log(`🔗 ${socket.id} joined lobby ${lobby}`);
      io.to(lobby).emit("chat", `📢 ${socket.id} has joined ${lobby}`);
    } catch (err) {
      console.error(`Error during join-lobby for socket ${socket.id}: ${err}`);
    }
  });

  // Forward map updates to all clients in the specified lobby
  socket.on("map-update", (data) => {
    try {
      if (data && data.lobby) {
        io.to(data.lobby).emit("map-update", data);
      } else {
        console.warn(`Invalid map-update data received from ${socket.id}`);
      }
    } catch (err) {
      console.error(`Error processing map-update from ${socket.id}: ${err}`);
    }
  });

  // Handle client disconnection
  socket.on("disconnect", () => {
    console.log(`❌ Client disconnected: ${socket.id}`);
  });
});

// Start the HTTP server and listen on the designated port
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`🚀 Conqueror server ready at http://localhost:${PORT}`);
});
