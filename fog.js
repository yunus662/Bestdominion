// fog.js
// Production‑Quality Fog‑of‑War Manager for Conqueror Engine
//
// This module implements a dynamic fog‑of‑war system for the game. It overlays a semi‑transparent canvas
// on top of the Leaflet map and clears “visible” regions based on the positions of units or known territory.
// You can enable, disable, and update the fog dynamically.
//
// Usage:
//   import { FogManager } from "./fog.js";
//   const fogManager = new FogManager(map);
//   fogManager.enable();
//   // Later, update the fog using an array of visible areas, each an object: { center: L.LatLng, radius: <meters> }
//   fogManager.updateFog(visibleAreas);
//   // To disable fog:
//   fogManager.disable();

class FogManager {
  constructor(map) {
    this.map = map;
    this.canvas = null;
    this.ctx = null;
    this.enabled = false;

    // Bind move/zoom update handler.
    this._onMapMove = this.update.bind(this);
  }

  // Enable the fog system by creating a canvas overlay covering the map.
  enable() {
    if (this.enabled) return;
    const mapContainer = this.map.getContainer();
    this.canvas = document.createElement("canvas");
    this.canvas.style.position = "absolute";
    this.canvas.style.top = 0;
    this.canvas.style.left = 0;
    this.canvas.style.pointerEvents = "none";
    this.canvas.style.zIndex = 500; // Ensure it overlays other layers.
    mapContainer.appendChild(this.canvas);
    this.ctx = this.canvas.getContext("2d");

    this._resizeCanvas();
    window.addEventListener("resize", () => this._resizeCanvas());
    this.map.on("moveend zoomend", this._onMapMove);
    this.enabled = true;

    // Initially fill the entire canvas with fog.
    this.updateFog([]);
  }

  // Disable fog by removing the canvas and listeners.
  disable() {
    if (!this.enabled) return;
    this.map.off("moveend zoomend", this._onMapMove);
    if (this.canvas && this.canvas.parentNode) {
      this.canvas.parentNode.removeChild(this.canvas);
    }
    this.canvas = null;
    this.ctx = null;
    this.enabled = false;
  }

  // Resize the canvas to always cover the map.
  _resizeCanvas() {
    if (!this.canvas) return;
    const size = this.map.getSize();
    this.canvas.width = size.x;
    this.canvas.height = size.y;
    // Redraw fog after resizing.
    this.updateFog([]);
  }

  /**
   * Update the fog-of-war.
   * @param {Array<Object>} visibleAreas - An array of visible areas. Each object should have:
   *    - center: A Leaflet LatLng object.
   *    - radius: Radius in meters indicating the visible circle.
   */
  updateFog(visibleAreas) {
    if (!this.ctx) return;
    const { width, height } = this.canvas;
    // Fill the entire canvas with semi‑transparent fog.
    this.ctx.fillStyle = "rgba(0, 0, 0, 0.6)";
    this.ctx.fillRect(0, 0, width, height);

    // For each visible area, clear a circular region from the fog.
    visibleAreas.forEach(area => {
      const { center, radius } = area;
      // Convert the center from LatLng to container point.
      const point = this.map.latLngToContainerPoint(center);
      // Convert the radius (meters) to pixels (approximation using map's current scale).
      const pixelRadius = this._metersToPixels(radius, center);
      this.ctx.save();
      this.ctx.beginPath();
      this.ctx.arc(point.x, point.y, pixelRadius, 0, 2 * Math.PI);
      this.ctx.clip();
      // Clear the clipped circle area (making it fully transparent).
      this.ctx.clearRect(point.x - pixelRadius, point.y - pixelRadius, pixelRadius * 2, pixelRadius * 2);
      this.ctx.restore();
    });
  }

  // A helper to update the fog without visible areas.
  update() {
    // In production, visibleAreas would be computed based on unit positions or territory visibility.
    // For now, reapply fog with no clearings.
    this.updateFog([]);
  }

  /**
   * Approximate conversion from meters to pixels at a given LatLng.
   * @param {number} meters
   * @param {L.LatLng} latlng
   * @returns {number} Approximate pixel distance.
   */
  _metersToPixels(meters, latlng) {
    // Use a small offset in longitude to estimate pixel scale.
    const pointA = this.map.latLngToContainerPoint(latlng);
    const pointB = this.map.latLngToContainerPoint(L.latLng(latlng.lat, latlng.lng + 0.0001));
    const metersPerPixel = 0.0001 / Math.abs(pointB.x - pointA.x);
    return meters / metersPerPixel;
  }
}

// Export the FogManager
export { FogManager };
