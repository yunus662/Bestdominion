export function resolveCombat(attacker, defender) {
  const baseDamage = attacker.damage;
  const defense = defender.defense;
  const net = baseDamage - (defense * 0.7) + Math.random() * 3;

  if (net > defense) return "attackerWins";
  if (net > defense / 2) return "stalemate";
  return "defenderHolds";
}
