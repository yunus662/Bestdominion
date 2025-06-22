// --- Do not modify these imports ---
import * as L from "https://unpkg.com/leaflet@1.9.4/dist/leaflet-src.esm.js";
import { Doctrines } from "./doctrine.js";
import { Governments } from "./government.js";
import { logEvent } from "./notification.js";
import { resolveCombat } from "./combat.js";
import { buyUnit, moveUnitTo } from "./units.js";
import { createFogLayer } from "./fog.js";
import { startGameClock } from "./time-engine.js";
import { economyManager } from "./econ-fixed.js";
import { allUnitsAI } from "./ai.js";
import { discoverResources } from "./survey.js";
import { buildings } from "./buildings.js";
import cities from "./cities.js";
import {
  initCountrySystem,
  upgradeInfrastructure,
  declareWar,
  makePeace,
  showNationPortfolio,
  getNationData
} from "./packed-features.js";
import { fullCountryData } from "./countries.js";
import { getCitiesWithRules, setCityOwner } from "./city-logic.js";

// --- End of import block ---

// PlayerStatsManager definition remains unchanged
class PlayerStatsManager {
  constructor() {
    this.tickets = 0;
    this.units = [];
    this.resources = economyManager.resources;
  }

  setUnits(units) {
    this.units = units.filter(Boolean);
  }

  earnTickets(amount) {
    this.tickets += amount;
    logEvent(`🎟️ Earned ${amount} ticket(s)!`, { type: "success", importance: 2 });
  }

  spendTickets(amount) {
    if (this.tickets >= amount) {
      this.tickets -= amount;
      logEvent(`💰 Spent ${amount} ticket(s)`, { type: "info" });
      return true;
    } else {
      logEvent("⚠️ Not enough tickets to purchase", { type: "warning", importance: 3 });
      return false;
    }
  }

  updateDisplay() {
    const res = this.resources;
    document.getElementById("stat-treasury").innerText = `Treasury: $${Math.floor(res.gold)}`;
    document.getElementById("stat-tickets").innerText = `🎟️ Tickets: ${this.tickets}`;
    document.getElementById("stat-military").innerText = `Military Units: ${this.units.length}`;
    document.getElementById("stat-food").innerText = `🍖 Food: ${Math.floor(res.food)}`;
    document.getElementById("stat-wood").innerText = `🪵 Wood: ${Math.floor(res.wood)}`;
    document.getElementById("stat-iron").innerText = `⛓️ Iron: ${Math.floor(res.iron || 0)}`;
    document.getElementById("stat-uranium").innerText = `☢️ Uranium: ${Math.floor(res.uranium || 0)}`;
    document.getElementById("stat-oil").innerText = `🛢️ Oil: ${Math.floor(res.oil || 0)}`;
    document.getElementById("stat-fuel").innerText = `⛽ Fuel: ${Math.floor(res.fuel || 0)}`;
    document.getElementById("stat-diamonds").innerText = `💎 Diamonds: ${Math.floor(res.diamonds || 0)}`;
  }
}

// Wrap everything into an async IIFE so we can use await for fetching geo data
document.addEventListener("DOMContentLoaded", () => {
  (async function() {
    console.log("engine.js: Starting execution...");

    // --- Initialize the map
    const map = L.map("map").setView([0, 0], 2);
    L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
      attribution: "© OpenStreetMap contributors"
    }).addTo(map);

    // --- Fetch country geo data (ensure countries.geo.json is in the correct location)
    async function loadCountryGeoData() {
      try {
        const response = await fetch("./countries.geo.json");
        const json = await response.json();
        console.log("countries.geo.json loaded:", json);
        return json;
      } catch (err) {
        console.error("❌ Error loading countries.geo.json:", err);
        return null;
      }
    }
    const geoData = await loadCountryGeoData();
    if (!geoData) {
      logEvent("❌ Could not load country data.", { type: "error" });
      return;
    }

    // --- Create a country selection overlay (UI similar to the start page)
    function showCountrySelectionOverlay(features) {
      const overlay = document.createElement("div");
      overlay.id = "country-selection-overlay";
      // These inline styles can be overridden in style.css for a uniform look.
      overlay.style.position = "fixed";
      overlay.style.top = "0";
      overlay.style.left = "0";
      overlay.style.width = "100%";
      overlay.style.height = "100%";
      overlay.style.background = "rgba(0, 0, 0, 0.85)";
      overlay.style.zIndex = "1500";
      overlay.style.display = "flex";
      overlay.style.flexDirection = "column";
      overlay.style.justifyContent = "center";
      overlay.style.alignItems = "center";
      overlay.style.color = "#00ffcc";
      overlay.style.fontFamily = "monospace";

      overlay.innerHTML = `
        <h2>Select Your Nation</h2>
        <select id="country-select" style="padding: 10px; font-size: 1.2em; margin: 10px;">
          ${features.map(feat => `<option value="${feat.properties.ADMIN}">${feat.properties.ADMIN}</option>`).join("")}
        </select>
        <button id="start-game-btn" style="padding: 10px 20px; font-size: 1.2em; margin-top: 20px; background: #e74c3c; border: none; border-radius: 4px; cursor: pointer;">
          Start Game
        </button>
      `;
      document.body.appendChild(overlay);
      return overlay;
    }
    const overlay = showCountrySelectionOverlay(geoData.features);

    const playerNation = await new Promise(resolve => {
      document.getElementById("start-game-btn").addEventListener("click", () => {
        const selected = document.getElementById("country-select").value;
        console.log("Country selected:", selected);
        resolve(selected);
      });
    });
    // Remove the selection overlay
    overlay.remove();

    // --- Create a game menu overlay (UI similar to the start page)
    function showGameMenu(nation) {
      const menu = document.createElement("div");
      menu.id = "game-menu";
      // Style this panel as needed via style.css or inline (the following is a minimal example)
      menu.style.position = "fixed";
      menu.style.top = "10px";
      menu.style.left = "10px";
      menu.style.background = "rgba(0, 0, 0, 0.7)";
      menu.style.color = "#00ffcc";
      menu.style.padding = "10px";
      menu.style.fontFamily = "monospace";
      menu.style.zIndex = "1100";
      menu.innerHTML = `
        <h2>${nation}</h2>
        <div id="resource-bar">
          <p id="stat-treasury">Treasury: $${Math.floor(economyManager.resources.gold)}</p>
          <p id="stat-tickets">🎟️ Tickets: 0</p>
          <p id="stat-military">Military Units: 0</p>
          <p id="stat-food">🍖 Food: ${Math.floor(economyManager.resources.food)}</p>
          <p id="stat-wood">🪵 Wood: ${Math.floor(economyManager.resources.wood)}</p>
        </div>
      `;
      document.body.appendChild(menu);
    }
    // Display the game menu using the selected nation
    showGameMenu(playerNation);

    // --- Initialize your country system using the chosen nation
    initCountrySystem(map);

    // --- Create player units using the selected nation
    const units = [
      buyUnit("infantry", [-1.3, 36.8], map, playerNation),
      buyUnit("aircraft", [30, 0], map, playerNation),
      buyUnit("warship", [0, 30], map, playerNation),
      buyUnit("trade", [0, 40], map, playerNation),
      buyUnit("helicopter", [5, 35], map, playerNation),
      buyUnit("tank", [12, 22], map, playerNation),
      buyUnit("artillery", [14, 24], map, playerNation),
      buyUnit("anti_air", [16, 26], map, playerNation),
      buyUnit("fighter", [18, 28], map, playerNation),
      buyUnit("bomber", [20, 30], map, playerNation),
      buyUnit("destroyer", [0, 25], map, playerNation),
      buyUnit("submarine", [0, 27], map, playerNation),
      buyUnit("transport", [0, 29], map, playerNation)
    ];

    const statsManager = new PlayerStatsManager();
    statsManager.setUnits(units);

    // --- Process city data: add markers and tooltips for each city per country
    const enrichedCountries = getCitiesWithRules();
    enrichedCountries.forEach(country => {
      country.cities.forEach(city => {
        const marker = L.marker([city.lat, city.lng]).addTo(map);
        marker.bindTooltip(
          `${city.name} (${city.owner})\nInfra: ${city.infrastructureLevel}, Econ: ×${city.economicMultiplier}`,
          { permanent: false }
        );
      });
    });

    // --- Create Fog-of-War layer and set periodic reveal if units exist
    const { reveal } = createFogLayer(map);
    if (units[0]) {
      setInterval(() => {
        reveal(units[0].getLatLng());
      }, 5000);
    }

    // --- Set up unit click handling with audio feedback
    const clickSound = new Audio("sounds/click.mp3");
    clickSound.volume = 0.5;
    let selectedUnit = null;

    units.forEach(unit => {
      unit?.on("click", (e) => {
        clickSound.play();
        selectedUnit = unit;
        logEvent(`🧭 Selected ${unit.unitType} for movement.`);
        e.originalEvent.stopPropagation();
      });
    });

    // --- Handle map clicks to move selected units
    map.on("click", (e) => {
      if (selectedUnit) {
        const dest = [e.latlng.lat, e.latlng.lng];
        moveUnitTo(selectedUnit, dest, selectedUnit.unitType, map);
        logEvent(`🛰️ ${selectedUnit.unitType} moving to [${dest[0].toFixed(2)}, ${dest[1].toFixed(2)}]`);
        selectedUnit = null;
      }
    });

    // --- Log doctrine and government details in the game log
    const doctrine = Doctrines["aggressive"];
    const government = Governments["republic"];
    logEvent(`📜 Doctrine: ${doctrine.name}`);
    logEvent(`🏛️ Government: ${government.name}`);

    // --- Optionally call AI initialization here if available:
    // allUnitsAI(map);

    // --- Start the in-game clock & update player stats display accordingly
    startGameClock((gameMinutes) => {
      if (gameMinutes % 1440 === 0) {
        logEvent("📆 A new in-game day has begun.");
      }
      statsManager.updateDisplay();
    });

    // --- Finally, remove the initial loading screen now that setup is complete.
    document.getElementById("loading-screen").style.display = "none";

    console.log("engine.js: Gameplay initialized successfully.");
  })();
});
