#!/usr/bin/env python3
"""
government.py
=============
A production‑quality governance module for the Conqueror Engine.

This module is responsible for modeling different forms of government along with
their associated benefits and drawbacks. It supports:
  • Defining a comprehensive Government class.
  • Maintaining a registry of government variants (Democracy, Republic, Oligarchy, Monarchy,
    Dictatorship, Technocracy, Plutocracy).
  • Simulating government transitions and elections.
  • Applying policy effects that modify economic and military bonuses.
  • Detailed logging for production‑quality debugging and future extension.

Usage:
  Import this module to retrieve government data and simulate government changes for nations.
  The module can also be run standalone to simulate a basic election and change process.
"""

import logging
import random
import time
from datetime import datetime

# -------------------------------------------------------------------------
# Configure Logging
# -------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(asctime)s] [%(levelname)s] %(message)s',
    datefmt='%H:%M:%S'
)

# -------------------------------------------------------------------------
# Government Class
# -------------------------------------------------------------------------
class Government:
    """
    Represents a government system with various efficiency and stability parameters.
    
    Attributes:
      gov_type (str): Internal identifier of government type (e.g., 'democracy').
      name (str): Display name.
      stability (float): A rating from 0 to 100 representing political stability.
      economic_bonus (float): Percentage bonus applied to national economic output.
      military_bonus (float): Percentage bonus applied to national military operations.
      description (str): A brief description of the government’s ideology and effects.
      last_policy_update (datetime): Timestamp of the last policy or stability change.
    """
    def __init__(self, gov_type: str, name: str, stability: float,
                 economic_bonus: float, military_bonus: float, description: str):
        self.gov_type = gov_type
        self.name = name
        self.stability = stability
        self.economic_bonus = economic_bonus
        self.military_bonus = military_bonus
        self.description = description
        self.last_policy_update = datetime.now()
        logging.debug(f"Initialized Government: {self}")

    def __str__(self):
        return (f"{self.name} (Stability: {self.stability:.1f}, "
                f"Economy Bonus: {self.economic_bonus}%, "
                f"Military Bonus: {self.military_bonus}%)")

    def update_stability(self, delta: float):
        """
        Adjusts the government stability by delta, clamping the result between 0 and 100.
        """
        old_stability = self.stability
        self.stability = max(0, min(100, self.stability + delta))
        self.last_policy_update = datetime.now()
        logging.debug(f"{self.name}: Stability adjusted from {old_stability:.1f} to {self.stability:.1f}")

    def simulate_election(self):
        """
        Simulate an election event that can alter stability randomly.
        High voter turnout or unforeseen events can either boost or damage stability.
        """
        change = random.uniform(-10, 15)  # Elections can swing stability (-10 to +15).
        logging.info(f"{self.name}: Election simulation change: {change:.1f}")
        self.update_stability(change)

    def apply_policy_effect(self, policy_effect: dict):
        """
        Applies a set of policy effects. The dictionary may contain keys:
          - 'economic_bonus_delta'
          - 'military_bonus_delta'
          - 'stability_delta'
        """
        econ_delta = policy_effect.get('economic_bonus_delta', 0)
        mil_delta = policy_effect.get('military_bonus_delta', 0)
        stab_delta = policy_effect.get('stability_delta', 0)
        old_econ = self.economic_bonus
        old_mil = self.military_bonus
        self.economic_bonus += econ_delta
        self.military_bonus += mil_delta
        self.update_stability(stab_delta)
        logging.info(f"{self.name}: Policy applied | Economic {old_econ}% -> {self.economic_bonus}% | "
                     f"Military {old_mil}% -> {self.military_bonus}%")

# -------------------------------------------------------------------------
# Global Registry of Government Types
# -------------------------------------------------------------------------
Government_types = {
    'democracy': {
        'name': 'Democracy',
        'stability': 80,
        'economic_bonus': 5,
        'military_bonus': 5,
        'description': 'Representative government with high public participation and transparency.'
    },
    'republic': {
        'name': 'Republic',
        'stability': 75,
        'economic_bonus': 7,
        'military_bonus': 4,
        'description': 'A state in which power rests in the hands of elected officials, balancing tradition and reform.'
    },
    'oligarchy': {
        'name': 'Oligarchy',
        'stability': 65,
        'economic_bonus': 4,
        'military_bonus': 6,
        'description': 'Rule by a select few; policies are efficiently executed but risk public discontent.'
    },
    'monarchy': {
        'name': 'Monarchy',
        'stability': 70,
        'economic_bonus': 5,
        'military_bonus': 7,
        'description': 'A historically rooted system with centralized leadership and deep-seated traditions.'
    },
    'dictatorship': {
        'name': 'Dictatorship',
        'stability': 60,
        'economic_bonus': 3,
        'military_bonus': 10,
        'description': 'An authoritarian regime that can mobilize resources rapidly at the expense of civil liberties.'
    },
    'technocracy': {
        'name': 'Technocracy',
        'stability': 85,
        'economic_bonus': 10,
        'military_bonus': 5,
        'description': 'Governance by experts; promotes innovation, robust economic performance, and evidence-based policy.'
    },
    'plutocracy': {
        'name': 'Plutocracy',
        'stability': 55,
        'economic_bonus': 8,
        'military_bonus': 4,
        'description': 'Rule by the wealthy, ensuring efficient capital mobilization but often leading to inequality.'
    }
}

# Global registry dictionary.
Governments = {}

def init_governments():
    """
    Initializes the Governments global dictionary with all predefined government types.
    """
    for key, props in Government_types.items():
        gov = Government(key, props['name'], props['stability'],
                         props['economic_bonus'], props['military_bonus'], props['description'])
        Governments[key] = gov
        logging.debug(f"Government registered: {gov}")

def get_government(gov_type: str) -> Government:
    """
    Retrieves a Government object by its type identifier.
    Returns None and logs an error if the government type is not found.
    """
    gov = Governments.get(gov_type)
    if gov is None:
        logging.error(f"Government type '{gov_type}' not found.")
    return gov

# -------------------------------------------------------------------------
# Additional: Government Manager for Simulating Multi-Nation Scenarios
# -------------------------------------------------------------------------
class GovernmentManager:
    """
    Simulates and manages government transitions for multiple nations.
    
    Each nation is simulated as a dictionary containing at least:
      - 'name': Nation name.
      - 'government': A Government object.
    """
    def __init__(self):
        self.nations = {}  # Maps nation name to its data dictionary.
        logging.debug("GovernmentManager initialized.")

    def register_nation(self, nation_name: str, initial_gov_type: str = 'democracy'):
        if nation_name in self.nations:
            logging.warning(f"Nation '{nation_name}' is already registered.")
            return
        gov = get_government(initial_gov_type)
        if gov is None:
            logging.error(f"Cannot register nation {nation_name} with unknown government '{initial_gov_type}'.")
            return
        self.nations[nation_name] = {'name': nation_name, 'government': gov}
        logging.info(f"Nation '{nation_name}' registered with government: {gov.name}")

    def change_government(self, nation_name: str, new_gov_type: str):
        """
        Simulates a government change (e.g., after elections or coups) for a nation.
        """
        if nation_name not in self.nations:
            logging.error(f"Nation '{nation_name}' is not registered.")
            return False
        new_gov = get_government(new_gov_type)
        if new_gov is None:
            logging.error(f"New government type '{new_gov_type}' is unknown.")
            return False
        old_gov = self.nations[nation_name].get('government')
        self.nations[nation_name]['government'] = new_gov
        logging.info(f"Nation '{nation_name}' changed government from "
                     f"{old_gov.name if old_gov else 'None'} to {new_gov.name}.")
        return True

    def simulate_national_election(self, nation_name: str):
        """
        Simulates an election for the specified nation, updating government stability.
        """
        if nation_name not in self.nations:
            logging.error(f"Nation '{nation_name}' is not registered for election simulation.")
            return
        gov = self.nations[nation_name].get('government')
        if gov:
            logging.info(f"Simulating election for '{nation_name}'...")
            gov.simulate_election()
        else:
            logging.error(f"Nation '{nation_name}' has no government assigned.")
    
    def dump_national_status(self):
        """
        Logs the current government status for all registered nations.
        """
        logging.info("---- National Government Status ----")
        for nation in self.nations.values():
            gov = nation.get('government')
            logging.info(f"{nation['name']}: {gov}")

# -------------------------------------------------------------------------
# Standalone Testing Block
# -------------------------------------------------------------------------
if __name__ == '__main__':
    logging.info("Starting government module standalone simulation.")
    init_governments()
    
    # Create a GovernmentManager and register several nations.
    gov_manager = GovernmentManager()
    gov_manager.register_nation("TestLand", "republic")
    gov_manager.register_nation("New Horizon", "technocracy")
    gov_manager.register_nation("Old Empire", "monarchy")
    
    # Dump initial status.
    gov_manager.dump_national_status()
    
    # Simulate national elections.
    gov_manager.simulate_national_election("TestLand")
    gov_manager.simulate_national_election("New Horizon")
    gov_manager.simulate_national_election("Old Empire")
    
    # Change government for one nation.
    time.sleep(1)
    gov_manager.change_government("Old Empire", "dictatorship")
    
    # Apply a policy effect to "TestLand".
    test_gov = gov_manager.nations["TestLand"]["government"]
    test_gov.apply_policy_effect({
        'economic_bonus_delta': +2,
        'military_bonus_delta': -1,
        'stability_delta': +5
    })
    
    # Final status dump.
    gov_manager.dump_national_status()
    
    logging.info("Government module simulation complete.")
