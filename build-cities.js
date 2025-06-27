// build-cities.js — generate cities.js from real data

import citiesData from 'cities15k.json' assert { type: 'json' };
import { countries as countryData } from 'country-data';
import fs from 'fs';

// 🔢 Define country tiers with city limits (strongest to weakest)
const tiers = [
  { name: 'Tier 1 (27 cities)', codes: ['US','CN','RU','DE','GB','FR','JP','IN','BR','IT'], max: 27 },
  { name: 'Tier 2 (20 cities)', codes: ['CA','AU','ES','MX','KR','TR','ID','NL','SA','CH'], max: 20 },
  { name: 'Tier 3 (12 cities)', codes: ['SE','PL','AR','BE','NO','ZA','NG','EG','TH','MY'], max: 12 },
];

// All other country codes (ensuring total = 195)
const allCodes = countryData.all.map(c => c.alpha2);
const usedCodes = tiers.flatMap(t => t.codes);
const restCodes = allCodes.filter(code => usedCodes.includes(code) && code !== undefined);

// Tier 4: weakest countries, up to a max of 4 cities
tiers.push({ name: 'Tier 4 (4 cities)', codes: restCodes, max: 4 });

// Group real city data by country code
const citiesByCountry = citiesData.reduce((map, { country, name, lat, lng, pop }) => {
  if (!map[country]) map[country] = [];
  map[country].push({ name, lat: +lat, lng: +lng, population: +pop });
  return map;
}, {});

// Generate output data
const outputCountries = [];

tiers.forEach(({ codes, max }) => {
  codes.forEach(code => {
    const countryInfo = countryData.all.find(c => c.alpha2 === code);
    if (!countryInfo) return;

    const list = (citiesByCountry[code] || [])
      .sort((a, b) => b.population - a.population)
      .slice(0, max);

    // Ensure at least one city: use capital if needed
    if (list.length === 0 && countryInfo.capital) {
      list.push({ name: countryInfo.capital, lat: 0, lng: 0, population: 0 });
    }

    outputCountries.push({
      name: countryInfo.name,
      code,
      cities: list.map(c => ({
        name: c.name,
        lat: +c.lat.toFixed(6),
        lng: +c.lng.toFixed(6),
        population: c.population
      }))
    });
  });
});

// Write to cities.js module
const jsModule = `export const countries = ${JSON.stringify(outputCountries, null, 2)};\n`;
fs.writeFileSync('src/cities.js', jsModule, 'utf8');
console.log(`✅ Generated cities.js with ${outputCountries.length} countries.`);
