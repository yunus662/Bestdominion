// infrastructure.js
// Production‑Quality Infrastructure Management for Conqueror Engine

/* 
   Overview:
     This module connects with the Leaflet map to manage and display national infrastructure.
     It handles:
        • Removal of existing roads (represented as Leaflet layers)
        • Upgrading infrastructure by spending in-game money/resources
        • Generating default infrastructure (roads) for nations that lack it
     
     Assumptions:
        - A global Leaflet map is available in the variable "map".
        - A function getNationData(nationName) exists and returns a nation object.
          A nation object should generally have properties:
                treasury (numeric), 
                infrastructureLevel (numeric; default 0 if undefined), 
                roadsLayer (a Leaflet LayerGroup to hold road features).
        - In-game money/resources are deducted from nation.treasury.
*/

// Assume that the Leaflet library is already loaded.
// Global variable "map" is assumed to be the Leaflet map instance.

class InfrastructureManager {
  constructor(map) {
    this.map = map;
    // Maintain an internal registry mapping nation names to their infrastructure data.
    // Each entry has {infrastructureLevel, roadsLayer}
    this.nationInfrastructure = {};
    console.log("[INFO] InfrastructureManager initialized.");
  }

  /**
   * Remove all road layers for all nations from the map.
   * For each nation's roadsLayer, remove it from the map if present.
   */
  removeAllRoads() {
    console.log("[INFO] Removing all roads for all nations...");
    for (const nationName in this.nationInfrastructure) {
      const infra = this.nationInfrastructure[nationName];
      if (infra.roadsLayer && this.map.hasLayer(infra.roadsLayer)) {
        this.map.removeLayer(infra.roadsLayer);
        console.log(`[DEBUG] Removed roads for nation ${nationName}.`);
      }
      // Optionally clear the roadsLayer from the registry.
      infra.roadsLayer = null;
    }
  }

  /**
   * Generates default infrastructure (road network) for a nation.
   * For production, developers should integrate actual geo data.
   * Here we simulate by drawing a random polyline.
   *
   * @param {string} nationName - The name of the nation.
   */
  generateInfrastructureForNation(nationName) {
    console.log(`[INFO] Generating infrastructure for ${nationName}...`);
    // For simulation: generate a polyline connecting random points.
    // In production, you could use nation's geographic boundaries.
    const latlngs = [];
    // For demonstration, generate 3 to 5 random points.
    const pointCount = 3 + Math.floor(Math.random() * 3);
    for (let i = 0; i < pointCount; i++) {
      // Generate random lat/lng around a center.
      // Assume each nation has a "center" property; if not, use fallback coordinates.
      let center = { lat: 0, lng: 0 };
      const nationData = getNationData(nationName) || {};
      if (nationData.center) {
        center = nationData.center;
      } else {
        // fallback random center for demonstration.
        center = { lat: 10 + Math.random() * 40, lng: -80 + Math.random() * 40 };
      }
      const lat = center.lat + (Math.random() - 0.5) * 5;
      const lng = center.lng + (Math.random() - 0.5) * 5;
      latlngs.push([lat, lng]);
    }
    // Create a polyline to simulate roads.
    const roadsLayer = L.polyline(latlngs, {
      color: 'gray',
      weight: 4,
      dashArray: '5, 10',
      opacity: 0.7
    }).addTo(this.map);
    
    // Save/update the nation's infrastructure record.
    if (!this.nationInfrastructure[nationName]) {
      this.nationInfrastructure[nationName] = { infrastructureLevel: 0, roadsLayer: null };
    }
    this.nationInfrastructure[nationName].roadsLayer = roadsLayer;
    console.log(`[INFO] Infrastructure generated for ${nationName}: Level ${this.nationInfrastructure[nationName].infrastructureLevel}`);
  }

  /**
   * Upgrades the infrastructure of a nation by spending in-game money/resources.
   * The upgrade cost is determined by a base value multiplied by the current level + 1.
   *
   * @param {string} nationName - The name of the nation.
   */
  upgradeInfrastructure(nationName) {
    // Retrieve nation data.
    const nation = getNationData(nationName);
    if (!nation) {
      console.error(`[ERROR] Nation ${nationName} data not found.`);
      return;
    }
    // Ensure the nation has an infrastructure record.
    if (!this.nationInfrastructure[nationName]) {
      // Initialize with default level 0.
      this.nationInfrastructure[nationName] = { infrastructureLevel: 0, roadsLayer: null };
    }
    const infra = this.nationInfrastructure[nationName];
    const baseCost = 100000;  // Base cost for an upgrade.
    const upgradeCost = baseCost * (infra.infrastructureLevel + 1);
    
    // Check if the nation has sufficient funds.
    if (nation.treasury < upgradeCost) {
      console.warn(`[WARNING] ${nationName} cannot afford upgrade. Required: $${upgradeCost.toLocaleString()}, Available: $${nation.treasury.toLocaleString()}`);
      return;
    }
    // Deduct the cost.
    nation.treasury -= upgradeCost;
    infra.infrastructureLevel++;
    console.log(`[INFO] ${nationName} upgraded infrastructure to level ${infra.infrastructureLevel}. Cost: $${upgradeCost.toLocaleString()}. New treasury balance: $${nation.treasury.toLocaleString()}`);
    
    // Remove old roads if any.
    if (infra.roadsLayer && this.map.hasLayer(infra.roadsLayer)) {
      this.map.removeLayer(infra.roadsLayer);
    }
    
    // Re-generate infrastructure based on upgraded level.
    // For a higher level, perhaps roads become thicker or more extensive.
    // Here we simulate by generating a new polyline.
    this.generateInfrastructureForNation(nationName);
  }

  /**
   * Updates the infrastructure display by re-generating it for nations that do not have it.
   * This can be called periodically to ensure that every nation has active infrastructure.
   */
  updateInfrastructureDisplay() {
    console.log("[INFO] Updating infrastructure display for all nations...");
    // Assume a global nations list exists. For demonstration, iterate over all nations returned by getNationData.
    // In a production system, you might maintain a global list or use an external source.
    const nationNames = Object.keys(getAllNationData());
    nationNames.forEach(nationName => {
      const nation = getNationData(nationName);
      if (!nation) return;
      if (!nation.infrastructureLevel || nation.infrastructureLevel === 0 || !this.nationInfrastructure[nationName]?.roadsLayer) {
        console.log(`[DEBUG] Nation ${nationName} has no active infrastructure. Generating...`);
        // Generate initial infrastructure.
        this.generateInfrastructureForNation(nationName);
      }
    });
  }
}

// --------------------------------------------------------------------------------
// Helper: getAllNationData()
// This is a stub function that should return an object containing all nation data.
// In production, this will be integrated with your overall nation management system.
function getAllNationData() {
  // For demonstration, we assume a global variable "nations" exists.
  // If not, return a sample.
  if (typeof nations !== "undefined") {
    return nations;
  }
  // Create sample nation data.
  return {
    "NationA": { name: "NationA", treasury: 5000000, center: { lat: 25, lng: -80 }, infrastructureLevel: 0 },
    "NationB": { name: "NationB", treasury: 4000000, center: { lat: 35, lng: -70 }, infrastructureLevel: 0 },
    "NationC": { name: "NationC", treasury: 6000000, center: { lat: 30, lng: -75 }, infrastructureLevel: 0 }
  };
}

// --------------------------------------------------------------------------------
// Helper: getNationData(nationName)
// This stub function returns a nation object for the specified nation.
// In production, replace with your actual nation management accessor.
function getNationData(nationName) {
  const allNations = getAllNationData();
  return allNations[nationName];
}

// --------------------------------------------------------------------------------
// Standalone Testing Block
// To run this file in standalone mode, include it in your index.html or load it in a browser console.
// You might want to test by calling functions from the InfrastructureManager.
if (typeof module !== "undefined" && module.exports) {
  // For module exporting in a build system.
  module.exports = InfrastructureManager;
} else {
  // If run directly in browser context:
  // Create an instance of InfrastructureManager once the Leaflet map has loaded.
  document.addEventListener("DOMContentLoaded", () => {
    // Assume the Leaflet map is already created and assigned to variable "map".
    if (typeof map === "undefined") {
      console.error("Leaflet map not found. Ensure 'map' is defined globally.");
      return;
    }
    const infraManager = new InfrastructureManager(map);
    // Update infrastructure display for all nations.
    infraManager.updateInfrastructureDisplay();

    // For demonstration, remove all roads after 10 seconds, then upgrade NationA's infrastructure.
    setTimeout(() => {
      console.log("[INFO] Removing all roads...");
      infraManager.removeAllRoads();
    }, 10000);

    setTimeout(() => {
      console.log("[INFO] Upgrading infrastructure for NationA...");
      infraManager.upgradeInfrastructure("NationA");
    }, 15000);

    // Optionally, set up a periodic refresh cycle:
    setInterval(() => {
      infraManager.updateInfrastructureDisplay();
    }, 30000);
  });
}
