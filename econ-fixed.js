// econ-fixed.js

import { logEvent } from "./notification.js";

// Define trade goods and their values
export const TradeGoods = {
  grain: { basePrice: 10, productionRate: 5 },
  oil: { basePrice: 25, productionRate: 2 },
  steel: { basePrice: 20, productionRate: 3 },
  electronics: { basePrice: 40, productionRate: 1 }
};

// Handle daily resource production
export function produceResources() {
  Object.entries(TradeGoods).forEach(([name, good]) => {
    const produced = good.productionRate;
    logEvent(`📦 Produced ${produced} units of ${name}`);
  });
}

logEvent("💰 Economy system initialized.");
