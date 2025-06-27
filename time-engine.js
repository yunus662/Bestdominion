// time_engine.js
// Production‑Quality Time Engine Module for Conqueror Engine
//
// This module defines a TimeEngine class which converts real time to game time
// using a configurable time acceleration factor. It emits "tick" events periodically,
// allowing other modules to update game state (such as resource accumulation, event processing,
// or unit movement scheduling) based on the simulated game time.
//
// Usage Example:
//   import { TimeEngine } from "./time_engine.js";
//   const engine = new TimeEngine({ realTimeFactor: 60, tickIntervalMs: 1000 });
//   engine.on("tick", (currentGameTime) => {
//     console.log(`Game time updated: ${currentGameTime.toISOString()}`);
//   });
//   engine.start();

import { EventEmitter } from "events";

export class TimeEngine extends EventEmitter {
  /**
   * Constructs a new TimeEngine instance.
   *
   * @param {Object} [options={}] - Configuration options.
   * @param {number} [options.realTimeFactor=60] - The factor by which real time is accelerated (e.g. 60 means every real second adds 60 game seconds).
   * @param {number} [options.tickIntervalMs=1000] - The real-time interval (in milliseconds) at which ticks occur.
   * @param {Date} [options.startTime=new Date()] - The starting game time.
   */
  constructor(options = {}) {
    super();
    this.realTimeFactor = options.realTimeFactor || 60; // e.g., 60 game seconds per real second.
    this.tickIntervalMs = options.tickIntervalMs || 1000; // default tick interval: 1 second (real time)
    this.currentGameTime = options.startTime || new Date();
    this.timer = null;
  }

  /**
   * Starts the game time engine. The engine will emit "tick" events at the specified interval.
   */
  start() {
    if (this.timer) return; // already running
    this.timer = setInterval(() => {
      // Calculate the game time advance in seconds.
      const realIntervalSeconds = this.tickIntervalMs / 1000;
      const gameAdvanceSeconds = realIntervalSeconds * this.realTimeFactor;
      // Update current game time.
      this.currentGameTime = new Date(this.currentGameTime.getTime() + gameAdvanceSeconds * 1000);
      // Emit the tick event with the updated game time.
      this.emit("tick", this.currentGameTime);
    }, this.tickIntervalMs);
    this.emit("started", this.currentGameTime);
  }

  /**
   * Stops the game time engine.
   */
  stop() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
      this.emit("stopped", this.currentGameTime);
    }
  }

  /**
   * Returns the current game time.
   *
   * @returns {Date} The current simulated game time.
   */
  getCurrentTime() {
    return this.currentGameTime;
  }
}
