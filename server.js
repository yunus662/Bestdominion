// server.js (Conqueror-games)

import express from "express";
import http from "http";
import { Server as SocketIO } from "socket.io";
import path from "path";
import { fileURLToPath } from "url";

// Normalize directory paths
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// Create server
const app = express();
const server = http.createServer(app);
const io = new SocketIO(server);

// Serve static files from Conqueror-games (including game.html, modules, etc.)
app.use(express.static(path.join(__dirname, ".")));

app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "game.html"));
});

// Handle lobby and multiplayer messaging
io.on("connection", (socket) => {
  console.log(`🛰️ Client connected: ${socket.id}`);

  socket.on("join-lobby", (lobby) => {
    socket.join(lobby);
    console.log(`🔗 ${socket.id} joined lobby ${lobby}`);
    io.to(lobby).emit("chat", `📢 ${socket.id} has joined ${lobby}`);
  });

  socket.on("map-update", (data) => {
    io.to(data.lobby).emit("map-update", data);
  });

  socket.on("disconnect", () => {
    console.log(`❌ ${socket.id} disconnected`);
  });
});

// Launch
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`🚀 Conqueror server ready at http://localhost:${PORT}`);
});
