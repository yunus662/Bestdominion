import { fullCountryData } from "./countries.js";
import { logEvent } from "./notification.js";
import {
  getCitiesWithRules,
  setCityOwner,
  setCityUpgrades
} from "./city-logic.js";

let currentNation = null;
const nations = {};

export function initCountrySystem(map) {
  createNationMenu();

  const world = getCitiesWithRules();
  world.forEach(country => {
    const gdpEntry = fullCountryData.find(c => c.name === country.name);
    const gdp = gdpEntry?.gdp || 100;
    const pop = gdpEntry?.population || 1_000_000;

    nations[country.name] = {
      name: country.name,
      gdp,
      population: pop,
      treasury: estimateStartingTreasury(gdp),
      dailyIncome: estimateIncome(gdp),
      infraLevel: 1,
      atWarWith: [],
      cities: country.cities.map(city => city.name),
      units: []
    };
  });

  // 💰 Accumulate income
  setInterval(() => {
    if (currentNation) {
      const nation = nations[currentNation];
      nation.treasury += nation.dailyIncome * nation.infraLevel;
    }
  }, 10_000);
}

function createNationMenu() {
  const menu = document.createElement("select");
  menu.id = "country-selector";

  fullCountryData.forEach(c => {
    const opt = document.createElement("option");
    opt.value = c.name;
    opt.textContent = c.name;
    menu.appendChild(opt);
  });

  const button = document.createElement("button");
  button.textContent = "Start Game";
  button.onclick = () => {
    const chosen = menu.value;
    currentNation = chosen;
    logEvent(`🌍 You are now leading ${chosen}`);
    showNationPortfolio(chosen);
  };

  document.body.append(menu, button);
}

export function upgradeInfrastructure(cityName) {
  const cityNation = getOwnerOf(cityName);
  if (cityNation !== currentNation) {
    logEvent(`⚠️ You can't upgrade ${cityName}; it's not under your control.`);
    return;
  }

  const nation = nations[currentNation];
  const cityLevel = getCityInfraLevel(cityName);
  const cost = cityLevel * 500_000;

  if (nation.treasury < cost) {
    logEvent(`⚠️ Not enough treasury to upgrade ${cityName}. Required: $${cost.toLocaleString()}`);
    return;
  }

  nation.treasury -= cost;
  const nextLevel = Math.min(cityLevel + 1, 5);
  const newMultiplier = 1 + nextLevel * 0.2;

  setCityUpgrades(cityName, {
    infrastructureLevel: nextLevel,
    roadDensity: nextLevel,
    economicMultiplier: newMultiplier
  });

  logEvent(`🏗️ ${cityName} upgraded to level ${nextLevel}.`);
}

export function declareWar(targetName) {
  const nation = nations[currentNation];
  const target = nations[targetName];
  if (!nation || !target) return;
  if (!nation.atWarWith.includes(targetName)) {
    nation.atWarWith.push(targetName);
    logEvent(`⚔️ ${nation.name} declared war on ${targetName}`);
  }
}

export function makePeace(targetName) {
  const nation = nations[currentNation];
  if (!nation) return;
  nation.atWarWith = nation.atWarWith.filter(n => n !== targetName);
  logEvent(`🕊️ ${nation.name} made peace with ${targetName}`);
}

export function conquerCity(cityName) {
  const prevOwner = getOwnerOf(cityName);
  if (prevOwner === currentNation) {
    logEvent(`${cityName} is already under your control.`);
    return;
  }

  const conquering = nations[currentNation];
  const loser = nations[prevOwner];
  if (!conquering || !loser) return;

  // Transfer ownership
  loser.cities = loser.cities.filter(name => name !== cityName);
  conquering.cities.push(cityName);
  setCityOwner(cityName, currentNation);

  logEvent(`🏴‍☠️ ${currentNation} has conquered ${cityName} from ${prevOwner}`);
}

export function showNationPortfolio(nationName) {
  const nation = nations[nationName];
  if (!nation) return;

  const box = document.createElement("div");
  box.id = "nation-portfolio";
  box.style.position = "absolute";
  box.style.top = "10px";
  box.style.right = "10px";
  box.style.background = "#fff";
  box.style.padding = "10px";
  box.style.border = "1px solid black";
  box.style.maxWidth = "300px";

  box.innerHTML = `
    <h3>${nation.name} Portfolio</h3>
    <p><strong>Population:</strong> ${nation.population.toLocaleString()}</p>
    <p><strong>GDP:</strong> $${nation.gdp.toFixed(2)}B</p>
    <p><strong>Daily Income:</strong> $${nation.dailyIncome.toLocaleString()}</p>
    <p><strong>Treasury:</strong> $${nation.treasury.toLocaleString()}</p>
    <p><strong>Infrastructure:</strong> ${nation.infraLevel}</p>
    <p><strong>Cities:</strong> ${nation.cities.length}</p>
    <ul style="max-height: 150px; overflow-y: auto; font-size: 0.9em;">
      ${nation.cities.map(c => `<li>${c}</li>`).join("")}
    </ul>
  `;

  document.body.appendChild(box);
}

export function getNationData(name) {
  return nations[name];
}

export function getAllCities() {
  const world = getCitiesWithRules();
  return world.flatMap(c =>
    c.cities.map(city => ({
      name: city.name,
      nation: c.name,
      latlng: [city.lat, city.lng]
    }))
  );
}

// 🔍 Helpers
function getCityInfraLevel(cityName) {
  const world = getCitiesWithRules().flatMap(c => c.cities);
  const match = world.find(c => c.name === cityName);
  return match?.infrastructureLevel || 1;
}

function getOwnerOf(cityName) {
  const world = getCitiesWithRules().flatMap(c => c.cities);
  const match = world.find(c => c.name === cityName);
  return match?.owner || "Unknown";
}

// 💰 Economy estimation
function estimateIncome(gdp) {
  if (gdp > 10000) return 20_000_000;
  if (gdp > 1000) return 5_000_000;
  if (gdp > 100) return 1_000_000;
  return 100_000;
}

function estimateStartingTreasury(gdp) {
  if (gdp > 10000) return 1_000_000_000;
  if (gdp > 1000) return 300_000_000;
  if (gdp > 100) return 80_000_000;
  return 20_000_000;
}
