export function offerAlliance(nationA, nationB) {
  nationA.ai.alliances.push(nationB.name);
  nationB.ai.alliances.push(nationA.name);
}

export function declareWar(nationA, nationB) {
  nationA.ai.enemies.push(nationB.name);
  nationB.ai.enemies.push(nationA.name);
}

export function embargo(nationA, nationB) {
  console.log(`📉 ${nationA.name} embargoes ${nationB.name}`);
}
