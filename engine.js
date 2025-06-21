import * as L from "https://unpkg.com/leaflet@1.9.4/dist/leaflet-src.esm.js";
import { Doctrines } from "./doctrine.js";
import { Governments } from "./government.js";
import { logEvent } from "./notification.js";
import { resolveCombat } from "./combat.js";
import { buyUnit, moveUnitTo } from "./units.js";
import { createFogLayer } from "./fog.js";
import { startGameClock } from "./time-engine.js";
import { produceResources } from "./econ-fixed.js";
import { allUnitsAI, initAI } from "./ai.js";
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

// ✅ NEW: unified city logic (rules + ownership + upgrades)
import { getCitiesWithRules, setCityOwner } from "./city-logic.js";

document.addEventListener("DOMContentLoaded", () => {
  const map = L.map("map").setView([0, 0], 2);
  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    attribution: "© OpenStreetMap contributors"
  }).addTo(map);

  fetch("countries.geo.json")
    .then(res => res.json())
    .then(data => {
      L.geoJSON(data, {
        style: feature => {
          const match = fullCountryData.find(c => c.name === feature.properties.name);
          return {
            color: match ? match.color : "#888",
            weight: 1,
            fillOpacity: 0.5
          };
        }
      }).addTo(map);
    });

  initCountrySystem(map);

  const playerNation = "United States";

  // 🪖 Units
  const infantry = buyUnit("infantry", [-1.3, 36.8], map, playerNation);
  const aircraft = buyUnit("aircraft", [30, 0], map, playerNation);
  const warship = buyUnit("warship", [0, 30], map, playerNation);
  const tradeShip = buyUnit("trade", [0, 40], map, playerNation);
  const helicopter = buyUnit("helicopter", [5, 35], map, playerNation);

  buyUnit("tank", [12, 22], map, playerNation);
  buyUnit("artillery", [14, 24], map, playerNation);
  buyUnit("anti_air", [16, 26], map, playerNation);
  buyUnit("fighter", [18, 28], map, playerNation);
  buyUnit("bomber", [20, 30], map, playerNation);
  buyUnit("destroyer", [0, 25], map, playerNation);
  buyUnit("submarine", [0, 27], map, playerNation);
  buyUnit("transport", [0, 29], map, playerNation);

  // 🧠 Cities with rules, upgrades, and ownership
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

  // 🌫️ Fog of war
  const { reveal } = createFogLayer(map);
  if (infantry) {
    setInterval(() => {
      reveal(infantry.getLatLng());
    }, 5000);
  }

  // 🔊 Unit selection
  const clickSound = new Audio("sounds/click.mp3");
  clickSound.volume = 0.5;
  let selectedUnit = null;

  [infantry, aircraft, warship, tradeShip, helicopter].forEach(unit => {
    if (!unit) return;
    unit.on("click", (e) => {
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
      logEvent(`🛰️ ${selectedUnit.unitType} moving to [${dest[0].toFixed(2)}, ${dest[1].toFixed(2)}].`);
      selectedUnit = null;
    }
  });

  // 📜 Doctrine and government
  const doctrine = getDoctrine("aggressive");
  const government = getGovernment("republic");
  logEvent(`📜 Doctrine: ${doctrine.name}`);
  logEvent(`🏛️ Government: ${government.name}`);

  // 🧠 AI
  initAI(map);

  // ⏳ Game clock and economy
  startGameClock((gameMinutes) => {
    if (gameMinutes % 1440 === 0) {
      produceResources();
      logEvent("📆 A new in-game day has begun.");
    }
  });

  // ✅ Hide loading screen
  document.getElementById("loading-screen").style.display = "none";
});
