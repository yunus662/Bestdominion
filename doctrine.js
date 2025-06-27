// doctrine.js
/**
 * doctrine.js
 * Production‑Quality Doctrine Module for Conqueror Engine
 *
 * This module manages strategic doctrines that nations can adopt to boost various
 * aspects of gameplay. Each doctrine offers modifiers to parameters like daily income,
 * treasury growth, unit cost, combat effectiveness, infrastructure efficiency, or diplomatic influence.
 *
 * Some advanced doctrines require a subscription or premium ticket to be applied.
 *
 * Exports:
 *   - getAvailableDoctrines: Returns an array of available doctrine objects.
 *   - applyDoctrine: Applies the selected doctrine’s modifiers to a nation.
 *   - upgradeDoctrine: Placeholder for upgrading an existing doctrine.
 */

const doctrines = [
  {
    name: "Economic Expansion",
    description: "Boost national GDP and treasury income by focusing on fiscal policies and open markets.",
    modifiers: {
      dailyIncomeMultiplier: 1.2,
      treasuryGrowth: 1.1
    },
    cost: 500_000,
    subscriptionRequired: false
  },
  {
    name: "Military Innovation",
    description: "Invest in advanced technologies and tactics to enhance unit combat effectiveness.",
    modifiers: {
      unitCostReduction: 0.9,
      combatEffectiveness: 1.15
    },
    cost: 750_000,
    subscriptionRequired: false
  },
  {
    name: "Infrastructure Modernization",
    description: "Upgrade road networks and production facilities to achieve higher economic returns.",
    modifiers: {
      infrastructureEfficiency: 1.3,
      dailyIncomeMultiplier: 1.1
    },
    cost: 1_000_000,
    subscriptionRequired: true
  },
  {
    name: "Diplomatic Engagement",
    description: "Foster international alliances and raise diplomatic influence to ease tensions.",
    modifiers: {
      diplomacyBonus: 1.2,
      warCostReduction: 0.85
    },
    cost: 600_000,
    subscriptionRequired: false
  },
  {
    name: "Cyber Warfare",
    description: "Leverage digital innovations to disrupt enemy networks and safeguard your nation.",
    modifiers: {
      enemyDisruption: 1.25,
      unitSurveillance: 1.2
    },
    cost: 1_500_000,
    subscriptionRequired: true
  }
];

/**
 * getAvailableDoctrines
 * @returns {Array<Object>} Array of doctrine objects available to nations.
 */
export function getAvailableDoctrines() {
  return doctrines;
}

/**
 * applyDoctrine
 * Applies the specified doctrine’s modifiers to the provided nation object.
 *
 * @param {Object} nation - The nation object to modify.
 * @param {string} doctrineName - The name of the doctrine to apply.
 * @returns {boolean} True if successful; false otherwise.
 */
export function applyDoctrine(nation, doctrineName) {
  const doctrine = doctrines.find(d => d.name === doctrineName);
  if (!doctrine) {
    console.error(`Doctrine "${doctrineName}" not found.`);
    return false;
  }
  
  // Check for premium access if required.
  if (doctrine.subscriptionRequired && !nation.subscriptionActive) {
    console.warn(`Doctrine "${doctrineName}" requires a premium subscription.`);
    return false;
  }

  // Apply modifiers (if present) to the nation's parameters.
  if (doctrine.modifiers.dailyIncomeMultiplier) {
    nation.dailyIncome *= doctrine.modifiers.dailyIncomeMultiplier;
  }
  if (doctrine.modifiers.treasuryGrowth) {
    nation.treasuryGrowth = doctrine.modifiers.treasuryGrowth;
  }
  if (doctrine.modifiers.unitCostReduction) {
    nation.unitCostMultiplier = doctrine.modifiers.unitCostReduction;
  }
  if (doctrine.modifiers.combatEffectiveness) {
    nation.combatEffectiveness = doctrine.modifiers.combatEffectiveness;
  }
  if (doctrine.modifiers.infrastructureEfficiency) {
    nation.infraEfficiency = doctrine.modifiers.infrastructureEfficiency;
  }
  if (doctrine.modifiers.diplomacyBonus) {
    nation.diplomacyBonus = doctrine.modifiers.diplomacyBonus;
  }
  if (doctrine.modifiers.warCostReduction) {
    nation.warCostMultiplier = doctrine.modifiers.warCostReduction;
  }
  if (doctrine.modifiers.enemyDisruption) {
    nation.enemyDisruption = doctrine.modifiers.enemyDisruption;
  }
  if (doctrine.modifiers.unitSurveillance) {
    nation.unitSurveillance = doctrine.modifiers.unitSurveillance;
  }

  console.info(`Doctrine "${doctrineName}" applied to nation "${nation.name}".`);
  return true;
}

/**
 * upgradeDoctrine
 * Placeholder for upgrading the doctrine of a nation. In a full production system,
 * this function would involve additional investment costs and enhanced modifiers.
 *
 * @param {Object} nation - The nation object.
 * @param {string} doctrineName - The doctrine to upgrade.
 * @returns {boolean} True if the upgrade is successful.
 */
export function upgradeDoctrine(nation, doctrineName) {
  console.info(`Upgrading doctrine "${doctrineName}" for nation "${nation.name}".`);
  // Implement upgrade logic here.
  return true;
}

