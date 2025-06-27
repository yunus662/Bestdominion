#!/usr/bin/env python3
"""
diplomacy.py
============
This module implements a comprehensive, production‑quality diplomacy system
for Conqueror Engine.

Features:
  • Representation of bilateral relations with dynamic trust and hostility
  • Treaty management for alliances, trade agreements, and peace treaties
  • A central DiplomacyManager that handles declarations of war, peace,
    alliance formation/breaking, and trade deals
  • Simulation of diplomatic cycles to model evolution in bilateral relations
  • Extensive logging for in‑depth debugging and system analysis

Classes:
  • DiplomaticRelation – encapsulates trust, hostility, and last interaction time
  • Treaty – defines the type, parties, valid duration, and terms of diplomatic accords
  • DiplomacyManager – central manager for tracking relations and treaties among nations,
    providing methods to declare war, make peace, form alliances, sign trade agreements, etc.

Usage:
  Instantiate a DiplomacyManager, register your nations, and use the various methods
  to simulate and modify diplomatic relations. Running this module standalone will
  simulate a full cycle of diplomatic events between several test nations.
"""

import logging
import random
import time
from datetime import datetime, timedelta
from collections import defaultdict

# -------------------------------------------------------------------------
# Configure Logging
# -------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(asctime)s] [%(levelname)s] %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# -------------------------------------------------------------------------
# DiplomaticRelation Class
# -------------------------------------------------------------------------
class DiplomaticRelation:
    """
    Represents the bilateral diplomatic relation between two nations.
    
    Attributes:
      nationA (str): One nation in the relationship.
      nationB (str): The other nation.
      trust (float): A value between 0 and 100 indicating trust level.
      hostility (float): A value between 0 and 100 indicating the level of hostility.
      last_interaction (datetime): Timestamp of the last diplomatic event.
    """
    def __init__(self, nationA: str, nationB: str):
        self.nationA = nationA
        self.nationB = nationB
        self.trust = random.uniform(30, 70)  # initial trust between 30 and 70
        self.hostility = random.uniform(20, 50)
        self.last_interaction = datetime.now()
        logging.debug(f"Initialized {self}")

    def update_interaction(self, delta_trust: float, delta_hostility: float):
        """Updates trust and hostility values based on a diplomatic event."""
        self.trust = max(0, min(100, self.trust + delta_trust))
        self.hostility = max(0, min(100, self.hostility + delta_hostility))
        self.last_interaction = datetime.now()
        logging.debug(f"Updated relation: {self}")

    def __str__(self):
        return (f"Relation({self.nationA} - {self.nationB}): "
                f"Trust = {self.trust:.2f}, Hostility = {self.hostility:.2f}")

# -------------------------------------------------------------------------
# Treaty Class
# -------------------------------------------------------------------------
class Treaty:
    """
    Represents a diplomatic treaty among nations.
    
    Attributes:
      treaty_type (str): Type of treaty ('alliance', 'trade', 'peace', etc.).
      parties (list): List of nations involved.
      start_date (datetime): When the treaty goes into effect.
      end_date (datetime): When the treaty expires.
      terms (dict): A dictionary outlining the agreement details.
    """
    def __init__(self, treaty_type: str, parties: list, duration_days: int, terms: dict):
        self.treaty_type = treaty_type.lower()
        self.parties = parties
        self.start_date = datetime.now()
        self.end_date = self.start_date + timedelta(days=duration_days)
        self.terms = terms
        logging.debug(f"Created treaty: {self}")

    def is_active(self) -> bool:
        """Return True if the treaty is still active."""
        active = datetime.now() <= self.end_date
        logging.debug(f"Treaty {self.treaty_type} active: {active}")
        return active

    def __str__(self):
        return (f"Treaty({self.treaty_type.capitalize()}) among {', '.join(self.parties)} "
                f"valid until {self.end_date.strftime('%Y-%m-%d')} with terms: {self.terms}")

# -------------------------------------------------------------------------
# DiplomacyManager Class
# -------------------------------------------------------------------------
class DiplomacyManager:
    """
    Central manager for diplomatic interactions.
    
    Manages bilateral relations, treaties, and diplomatic events between nations.
    """
    def __init__(self):
        # Relations stored as (nationA, nationB) tuple in alphabetical order.
        self.relations = {}
        self.treaties = []
        self.nations = set()
    
    def register_nation(self, nation: str):
        """Registers a nation in the diplomatic database."""
        self.nations.add(nation)
        logging.info(f"Nation registered: {nation}")
    
    def _get_relation_key(self, nationA: str, nationB: str):
        """Returns an ordered tuple for consistency."""
        return tuple(sorted([nationA, nationB]))
    
    def get_relation(self, nationA: str, nationB: str) -> DiplomaticRelation:
        """Retrieves or creates a DiplomaticRelation between two nations."""
        key = self._get_relation_key(nationA, nationB)
        if key not in self.relations:
            self.relations[key] = DiplomaticRelation(nationA, nationB)
            logging.info(f"New relation established: {self.relations[key]}")
        return self.relations[key]
    
    def adjust_relation(self, nationA: str, nationB: str, delta_trust: float, delta_hostility: float):
        """Adjusts the bilateral relation between two nations."""
        relation = self.get_relation(nationA, nationB)
        relation.update_interaction(delta_trust, delta_hostility)
    
    def declare_war(self, aggressor: str, target: str):
        """
        Handles war declaration: reduces trust, increases hostility, and removes any peace treaties.
        """
        self.register_nation(aggressor)
        self.register_nation(target)
        self.adjust_relation(aggressor, target, delta_trust=-20, delta_hostility=+30)
        self._remove_treaty_between(aggressor, target, "peace")
        logging.info(f"{aggressor} has declared war on {target}.")
    
    def make_peace(self, nationA: str, nationB: str):
        """
        Facilitates peace talks: increases trust, decreases hostility, and records a peace treaty.
        """
        self.register_nation(nationA)
        self.register_nation(nationB)
        self.adjust_relation(nationA, nationB, delta_trust=+25, delta_hostility=-25)
        treaty = Treaty("peace", [nationA, nationB], duration_days=180, terms={"ceasefire": True})
        self.treaties.append(treaty)
        logging.info(f"{nationA} and {nationB} sign a peace treaty: {treaty}")
    
    def form_alliance(self, nationA: str, nationB: str):
        """
        Forms an alliance: significantly increases trust, decreases hostility, records an alliance treaty.
        """
        self.register_nation(nationA)
        self.register_nation(nationB)
        self.adjust_relation(nationA, nationB, delta_trust=+30, delta_hostility=-20)
        treaty = Treaty("alliance", [nationA, nationB], duration_days=365, terms={"mutual defense": True})
        self.treaties.append(treaty)
        logging.info(f"Alliance formed between {nationA} and {nationB}: {treaty}")
    
    def break_alliance(self, nationA: str, nationB: str):
        """
        Breaks an existing alliance: lowers trust, increases hostility, and removes the alliance treaty.
        """
        self.adjust_relation(nationA, nationB, delta_trust=-30, delta_hostility=+20)
        self._remove_treaty_between(nationA, nationB, "alliance")
        logging.info(f"The alliance between {nationA} and {nationB} has been terminated.")
    
    def sign_trade_agreement(self, nationA: str, nationB: str, terms: dict):
        """
        Signs a trade treaty: slightly increases trust and reduces hostility.
        """
        self.register_nation(nationA)
        self.register_nation(nationB)
        self.adjust_relation(nationA, nationB, delta_trust=+10, delta_hostility=-5)
        treaty = Treaty("trade", [nationA, nationB], duration_days=365, terms=terms)
        self.treaties.append(treaty)
        logging.info(f"Trade agreement between {nationA} and {nationB}: {treaty}")
    
    def _remove_treaty_between(self, nationA: str, nationB: str, treaty_type: str):
        """
        Removes active treaties of a given type between two nations.
        """
        before = len(self.treaties)
        key_set = {nationA, nationB}
        self.treaties = [t for t in self.treaties if not (
            t.treaty_type == treaty_type and set(t.parties) == key_set and t.is_active()
        )]
        after = len(self.treaties)
        logging.debug(f"Removed {before - after} {treaty_type} treaties between {nationA} and {nationB}.")
    
    def simulate_diplomatic_cycle(self, cycle_count: int = 10):
        """
        Simulates a series of diplomatic cycles with random fluctuations in trust and hostility.
        """
        logging.info("Beginning diplomatic cycle simulation...")
        for cycle in range(cycle_count):
            logging.info(f"Cycle {cycle + 1}/{cycle_count}")
            for relation in self.relations.values():
                delta_trust = random.uniform(-5, 5)
                delta_hostility = random.uniform(-5, 5)
                relation.update_interaction(delta_trust, delta_hostility)
            time.sleep(0.5)
        logging.info("Diplomatic cycle simulation complete.")
    
    def list_active_treaties(self):
        """
        Returns all active treaties.
        """
        active = [t for t in self.treaties if t.is_active()]
        for treaty in active:
            logging.info(str(treaty))
        return active
    
    def dump_state(self):
        """
        Logs the current state of all diplomatic relations and active treaties.
        """
        logging.info("---- Diplomatic Relations ----")
        for relation in self.relations.values():
            logging.info(str(relation))
        logging.info("---- Active Treaties ----")
        self.list_active_treaties()

# -------------------------------------------------------------------------
# Standalone Testing and Simulation
# -------------------------------------------------------------------------
if __name__ == '__main__':
    logging.info("Starting DiplomacyManager standalone simulation...")
    manager = DiplomacyManager()
    # Register test nations.
    manager.register_nation("NationA")
    manager.register_nation("NationB")
    manager.register_nation("NationC")
    
    # Initial state dump.
    manager.dump_state()
    
    # Simulate some diplomatic events.
    manager.declare_war("NationA", "NationB")
    time.sleep(1)
    manager.form_alliance("NationB", "NationC")
    manager.sign_trade_agreement("NationA", "NationC", terms={"oil": "100 barrels/month", "tech": "restricted"})
    time.sleep(1)
    manager.dump_state()
    
    # Run a few diplomatic cycles.
    manager.simulate_diplomatic_cycle(5)
    
    # Make peace and break the alliance.
    manager.make_peace("NationA", "NationB")
    manager.break_alliance("NationB", "NationC")
    manager.dump_state()
    
    logging.info("DiplomacyManager simulation complete.")
