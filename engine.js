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

document.addEventListener("DOMContentLoaded", () => {
  const map = L.map("map").setView([0, 0], 2);
  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    attribution: "© OpenStreetMap contributors"
  }).addTo(map);

  const playerNation = "United States";
  initCountrySystem(map);

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

  const { reveal } = createFogLayer(map);
  if (units[0]) {
    setInterval(() => {
      reveal(units[0].getLatLng());
    }, 5000);
  }

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

  map.on("click", (e) => {
    if (selectedUnit) {
      const dest = [e.latlng.lat, e.latlng.lng];
      moveUnitTo(selectedUnit, dest, selectedUnit.unitType, map);
      logEvent(`🛰️ ${selectedUnit.unitType} moving to [${dest[0].toFixed(2)}, ${dest[1].toFixed(2)}]`);
      selectedUnit = null;
    }
  });

  const doctrine = Doctrines["aggressive"];
  const government = Governments["republic"];
  logEvent(`📜 Doctrine: ${doctrine.name}`);
  logEvent(`🏛️ Government: ${government.name}`);

  initAI(map);

  startGameClock((gameMinutes) => {
    if (gameMinutes % 1440 === 0) {
      logEvent("📆 A new in-game day has begun.");
    }
    statsManager.updateDisplay();
  });

  document.getElementById("loading-screen").style.display = "none";
});
