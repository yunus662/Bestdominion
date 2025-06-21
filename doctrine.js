import { logEvent } from "./notification.js";
export const Doctrines = {
  SHOCK_AND_AWE: {
    name: "Shock and Awe",
    description: "Increased damage and morale under fire",
    bonus: "damage",
    multiplier: 1.25
  },
  ASYMMETRIC: {
    name: "Asymmetric Warfare",
    description: "Faster ground units and hidden movement",
    bonus: "speed",
    multiplier: 1.5
  },
  NAVAL_DOMINANCE: {
    name: "Naval Dominance",
    description: "Boosts range and damage of navy units",
    bonus: "naval",
    multiplier: 1.4
  },
  FORTRESS_DEFENSE: {
    name: "Fortress Defense",
    description: "Increased defense for cities and infantry",
    bonus: "defense",
    multiplier: 1.3
  }
};
