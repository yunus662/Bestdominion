// units.js
import * as L from "https://unpkg.com/leaflet@1.9.4/dist/leaflet-src.esm.js";
import { logEvent } from "./notification.js";
import { getNationData } from "./packed-features.js";

const unitTypes = {
  infantry: { cost: 100000, icon: "icons/infantry.png" },
  aircraft: { cost: 500000, icon: "icons/aircraft.png" },
  warship: { cost: 800000, icon: "icons/warship.png" },
  trade: { cost: 300000, icon: "icons/trade-ship.png" },
  helicopter: { cost: 400000, icon: "icons/helicopter.png" },
  tank: { cost: 250000, icon: "icons/tank.png" },
  artillery: { cost: 200000, icon: "icons/artillery.png" },
  anti_air: { cost: 180000, icon: "icons/anti-air.png" },
  fighter: { cost: 600000, icon: "icons/fighter.png" },
  bomber: { cost: 700000, icon: "icons/bomber.png" },
  destroyer: { cost: 900000, icon: "icons/warship.png" },
  submarine: { cost: 950000, icon: "icons/submarine.png" },
  transport: { cost: 350000, icon: "icons/transport.png" }
};

export function buyUnit(unitType, latlng, map, nationName) {
  const nation = getNationData(nationName);
  const unitInfo = unitTypes[unitType];
  if (!nation || !unitInfo) return;

  if (nation.treasury >= unitInfo.cost) {
    nation.treasury -= unitInfo.cost;
    const unit = createUnit(unitType, latlng, unitInfo.icon, map);
    nation.units = nation.units || [];
    nation.units.push(unit);
    logEvent(`🪖 ${nationName} purchased ${unitType} for $${unitInfo.cost.toLocaleString()}`);
    return unit;
  } else {
    logEvent(`⚠️ ${nationName} cannot afford ${unitType} ($${unitInfo.cost.toLocaleString()})`);
    return null;
  }
}

export function createUnit(type, latlng, iconUrl, map) {
  const icon = L.icon({
    iconUrl,
    iconSize: [32, 32],
    iconAnchor: [16, 16]
  });

  const marker = L.marker(latlng, { icon }).addTo(map);
  marker.unitType = type;
  return marker;
}

export function moveUnitTo(unit, destination, type, map) {
  if (!unit || typeof unit.setLatLng !== "function") {
    console.warn("Invalid unit passed to moveUnitTo:", unit);
    return;
  }

  unit.setLatLng(destination);
  // Future: Animate movement or adjust behavior based on type
}


