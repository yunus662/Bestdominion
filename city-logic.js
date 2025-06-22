import cities from "./data/cities.js";

/** Tiers and rules */
const cityRules = {
  rich: {
    countries: [ "US", "DE", "JP", "GB", "FR", "CA", "AU", "KR", "NL", "SE", "CH", "NO", "FI", "AT", "BE", "IE", "SG", "IS", "LU", "DK", "NZ", "AE", "QA", "SA", "IL", "TW", "CZ", "SI", "EE", "LT", "LV", "MT", "CY" ],
    rules: { infrastructureLevel: 4, hasRealRoads: true, roadDensity: 4, economicMultiplier: 1.6 }
  },
  middle: {
    countries: [ "CN", "IN", "BR", "MX", "ID", "TR", "RU", "TH", "MY", "PH", "PL", "AR", "CL", "CO", "PE", "KZ", "EG", "ZA", "DZ", "IR", "MA", "VN", "RO", "UA", "SR", "GE", "AM", "AZ", "BO", "EC", "UY", "TN", "LB", "JO", "AL", "RS", "BA", "ME", "MK", "MD", "GH", "NG", "KE", "TZ", "PK", "BD", "LK", "MM", "KH", "LA", "MN", "FJ", "PG", "VU", "SB", "NA", "BW", "ZM", "ZW", "GN", "GA", "BJ", "CM", "SN", "ML", "BF", "TD", "NE", "CI", "RW", "MG", "LS", "SZ", "GT", "HN", "SV", "NI", "PA", "DO", "PY", "GY", "TT", "JM", "MU", "CV", "PS", "BI" ],
    rules: { infrastructureLevel: 2, hasRealRoads: true, roadDensity: 2, economicMultiplier: 1.2 }
  },
  poor: {
    countries: [ "AF", "HT", "YE", "SD", "SS", "CD", "CF", "SO", "ER", "SL", "LR", "GM", "MR", "DJ", "MW", "MZ", "UG", "TG", "GW", "KM", "NP", "BT", "CG", "GQ", "TO", "TV", "KI", "FM", "MH", "NR", "ST", "TL", "MG", "ET", "NG", "ML", "NE" ],
    rules: { infrastructureLevel: 1, hasRealRoads: false, roadDensity: 1, economicMultiplier: 0.8 }
  }
};

/** Per-city customization */
const cityOverrides = {
  "Berlin": {
    owner: "Germany",
    upgrades: { infrastructureLevel: 5, roadDensity: 5, economicMultiplier: 2.0 }
  },
  "Mumbai": {
    upgrades: { infrastructureLevel: 3, roadDensity: 3, economicMultiplier: 1.4 }
  },
  "Addis Ababa": {
    upgrades: { infrastructureLevel: 2, roadDensity: 2, economicMultiplier: 1.1 }
  },
  "São Paulo": {
    upgrades: { infrastructureLevel: 4, roadDensity: 3, economicMultiplier: 1.6 }
  },
  "Cairo": {
    owner: "Egypt",
    upgrades: { infrastructureLevel: 3, roadDensity: 2, economicMultiplier: 1.3 }
  }
};

function getRulesForCode(code) {
  for (const tier of Object.values(cityRules)) {
    if (tier.countries.includes(code)) return tier.rules;
  }
  return { infrastructureLevel: 1, hasRealRoads: false, roadDensity: 1, economicMultiplier: 1.0 };
}

function getOwner(cityName, fallback) {
  return cityOverrides[cityName]?.owner || fallback;
}

function getUpgrade(cityName) {
  return cityOverrides[cityName]?.upgrades || null;
}

/** 🧭 Dynamic API */
export function setCityOwner(cityName, newOwner) {
  if (!cityOverrides[cityName]) cityOverrides[cityName] = {};
  cityOverrides[cityName].owner = newOwner;
}

export function setCityUpgrades(cityName, upgrades) {
  if (!cityOverrides[cityName]) cityOverrides[cityName] = {};
  cityOverrides[cityName].upgrades = upgrades;
}

export function getCitiesByOwner(nation) {
  return getCitiesWithRules()
    .flatMap(c => c.cities)
    .filter(city => city.owner === nation);
}

/** 🧩 Main export: full enriched data */
export function getCitiesWithRules() {
  return cities.map(country => {
    const rules = getRulesForCode(country.code);
    return {
      ...country,
      cities: country.cities.map(city => {
        const upgrades = getUpgrade(city.name);
        const owner = getOwner(city.name, country.name);
        return {
          ...city,
          ...rules,
          ...upgrades,
          owner,
          nation: country.name
        };
      })
    };
  });
}

/** 🎛️ New: Panel data hook */
export function getCityPanelData(cityName) {
  const matched = getCitiesWithRules().flatMap(c => c.cities).find(c => c.name === cityName);
  return matched || null;
}
