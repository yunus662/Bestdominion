#!/usr/bin/env python3
"""
country_relations.py
====================
Production‑Quality Country Relations and Border Simulation Module for Conqueror Engine

Overview:
  This module simulates territorial borders and movement restrictions based on diplomatic relationships.
  It enforces that units may only traverse into:
    • Their own territory (always allowed)
    • Enemy territory when nations are at war (allowed to capture and gradually shift borders)
    • Ally territory (allowed to move but not capture)
    • Neutral territory is off‑limits

  The module uses a simple rectangular model to represent each nation’s borders and maintains a
  relationship registry (ally, neutral, at_war). It also simulates gradual border shifts when units
  from a nation at war deploy past their original border.

Classes:
  • Territory – Represents a nation’s territory using rectangular boundaries.
  • CountryRelationsManager – Manages territories and relationships, checks movement permissions,
      and simulates border movement.
      
Usage:
  Import this module and use the CountryRelationsManager to check if a unit (from nation A) can enter
  a coordinate. You can also simulate border shifting when enemy forces capture territory.

Standalone:
  Running this module standalone will simulate a small scenario with three nations, set their
  relationships, and test unit movement–based permissions and border adjustments.
"""

import logging
from typing import Tuple, Optional, Dict
import time
import random

# -------------------------------------------------------------------------
# Logging Configuration
# -------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(asctime)s] [%(levelname)s] %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# -------------------------------------------------------------------------
# Territory Class
# -------------------------------------------------------------------------
class Territory:
    """
    Represents a nation's territory as a rectangular bounding box.
    
    Attributes:
      owner (str): Name of the nation.
      min_lat (float), max_lat (float): Latitude boundaries.
      min_lng (float), max_lng (float): Longitude boundaries.
    """
    def __init__(self, owner: str, min_lat: float, max_lat: float, min_lng: float, max_lng: float):
        self.owner = owner
        self.min_lat = min_lat
        self.max_lat = max_lat
        self.min_lng = min_lng
        self.max_lng = max_lng

    def contains(self, point: Tuple[float, float]) -> bool:
        lat, lng = point
        return self.min_lat <= lat <= self.max_lat and self.min_lng <= lng <= self.max_lng

    def shift_border(self, direction: str, amount: float):
        """
        Shifts the border in one direction by 'amount'. This simulates gradual border movement.
        Valid directions: 'north', 'south', 'east', 'west'.
        """
        if direction == 'north':
            self.max_lat += amount
        elif direction == 'south':
            self.min_lat -= amount
        elif direction == 'east':
            self.max_lng += amount
        elif direction == 'west':
            self.min_lng -= amount
        logging.info(f"{self.owner} border shifted {direction} by {amount} units.")

    def __str__(self):
        return (f"{self.owner} Territory: Lat[{self.min_lat}, {self.max_lat}], "
                f"Lng[{self.min_lng}, {self.max_lng}]")

# -------------------------------------------------------------------------
# CountryRelationsManager Class
# -------------------------------------------------------------------------
class CountryRelationsManager:
    """
    Manages border data and diplomatic relationships between nations.
    
    Key responsibilities:
      - Store territory boundaries for each nation.
      - Store relationship status (ally, neutral, at_war) between pairs of nations.
      - Determine the owner of a coordinate.
      - Determine if a unit (from a nation) can enter a given coordinate based on relationships.
      - Simulate border movement (capturing enemy territory) when at war.
    """
    
    def __init__(self):
        # Territories: mapping from nation name to Territory objects.
        self.territories: Dict[str, Territory] = {}
        # Relationships: keys are frozensets of two nation names, values are relationship strings.
        # Relationships are symmetrical.
        self.relationships: Dict[frozenset, str] = {}
        logging.info("CountryRelationsManager initialized.")

    def set_territory(self, nation: str, territory: Territory):
        self.territories[nation] = territory
        logging.info(f"Set territory for {nation}: {territory}")

    def set_relationship(self, nation1: str, nation2: str, status: str):
        """
        Sets the relationship between nation1 and nation2.
        status should be one of: 'ally', 'neutral', 'at_war'.
        """
        key = frozenset({nation1, nation2})
        self.relationships[key] = status
        logging.info(f"Relationship set between {nation1} and {nation2}: {status}")

    def get_relationship(self, nation1: str, nation2: str) -> str:
        key = frozenset({nation1, nation2})
        return self.relationships.get(key, "neutral")

    def territory_owner(self, point: Tuple[float, float]) -> Optional[str]:
        """
        Returns the name of the nation that owns the territory containing the point.
        If no territory contains the point, returns None.
        """
        for nation, territory in self.territories.items():
            if territory.contains(point):
                logging.debug(f"Point {point} is within {territory}")
                return nation
        logging.debug(f"Point {point} does not fall within any known territory.")
        return None

    def can_unit_enter(self, unit_nation: str, target: Tuple[float, float], is_capturing: bool = False) -> bool:
        """
        Determines if a unit from unit_nation can enter the territory at target.
          - Always allow if the territory belongs to unit_nation.
          - For enemy territory: allow only if the relationship is 'at_war'.
          - For allied territory: allow movement but disallow capturing (is_capturing).
          - For neutral territory: disallow entry.
        """
        owner = self.territory_owner(target)
        if owner is None:
            logging.debug("No territory owner for target point; assuming unclaimed – entry allowed.")
            return True  # Unclaimed territory is open.
        if owner == unit_nation:
            logging.debug(f"Target belongs to {unit_nation} – entry allowed.")
            return True
        rel = self.get_relationship(unit_nation, owner)
        if rel == "at_war":
            logging.debug(f"{unit_nation} is at war with {owner} – entry allowed and capturing permitted.")
            return True
        if rel == "ally":
            if is_capturing:
                logging.debug(f"{unit_nation} cannot capture allied territory ({owner}).")
                return False
            logging.debug(f"{unit_nation} is allied with {owner} – entry allowed (no capture).")
            return True
        # Neutral or any other relationship: disallow entry.
        logging.debug(f"{unit_nation} has neutral or undefined relationship with {owner} – entry denied.")
        return False

    def simulate_border_movement(self, attacker_nation: str, target: Tuple[float, float]):
        """
        When a unit from attacker_nation successfully deploys into enemy territory (if at war),
        simulate a gradual border shift. For simplicity, this function shifts the border of
        the enemy nation slightly toward the enemy's interior.
        
        For example, if NationA (attacker) moves into NationB's territory, and they are at war,
        reduce NationB's territory by shifting its border inward (based on the approximate
        direction from the enemy’s capital to the target point).
        """
        enemy = self.territory_owner(target)
        if enemy is None or enemy == attacker_nation:
            logging.debug("No border capture simulation required.")
            return

        # Check relationship; only allow capturing if at war.
        rel = self.get_relationship(attacker_nation, enemy)
        if rel != "at_war":
            logging.debug(f"No border movement: {attacker_nation} is not at war with {enemy}.")
            return

        # For simulation, assume a simple effect: shift enemy's border by a fixed small amount.
        enemy_territory = self.territories.get(enemy)
        if enemy_territory is None:
            logging.error(f"Enemy territory for {enemy} not found!")
            return

        # Determine the predominant direction of the target relative to enemy territory center.
        center_lat = (enemy_territory.min_lat + enemy_territory.max_lat) / 2
        center_lng = (enemy_territory.min_lng + enemy_territory.max_lng) / 2
        target_lat, target_lng = target

        # Calculate differences.
        diff_lat = target_lat - center_lat
        diff_lng = target_lng - center_lng

        # Based on differences, decide which border to move inward.
        # (This is a simplified heuristic: move the closest border.)
        if abs(diff_lat) > abs(diff_lng):
            # Move north or south border.
            if diff_lat > 0:
                # Attacker penetrated northern part: move enemy's north border southward.
                enemy_territory.max_lat -= 0.1
                logging.info(f"{attacker_nation} gains ground on {enemy}'s northern border.")
            else:
                enemy_territory.min_lat += 0.1
                logging.info(f"{attacker_nation} gains ground on {enemy}'s southern border.")
        else:
            # Move east or west border.
            if diff_lng > 0:
                enemy_territory.max_lng -= 0.1
                logging.info(f"{attacker_nation} gains ground on {enemy}'s eastern border.")
            else:
                enemy_territory.min_lng += 0.1
                logging.info(f"{attacker_nation} gains ground on {enemy}'s western border.")
        logging.info(f"Updated enemy territory: {enemy_territory}")

# -------------------------------------------------------------------------
# Standalone Simulation for CountryRelations
# -------------------------------------------------------------------------
if __name__ == '__main__':
    logging.info("Starting standalone simulation for country_relations.py")
    
    # Create a CountryRelationsManager instance.
    crm = CountryRelationsManager()
    
    # Define territories for three nations.
    # For simplicity, use rectangular boundaries.
    crm.set_territory("NationA", Territory("NationA", min_lat=10.0, max_lat=20.0, min_lng=10.0, max_lng=20.0))
    crm.set_territory("NationB", Territory("NationB", min_lat=15.0, max_lat=25.0, min_lng=18.0, max_lng=28.0))
    crm.set_territory("NationC", Territory("NationC", min_lat=5.0, max_lat=15.0, min_lng=25.0, max_lng=35.0))
    
    # Set up relationships.
    crm.set_relationship("NationA", "NationB", "at_war")
    crm.set_relationship("NationA", "NationC", "ally")
    crm.set_relationship("NationB", "NationC", "neutral")
    
    # Test cases:
    test_points = [
        ((12, 12), "NationA"),  # clearly in NationA territory
        ((17, 19), "NationA or NationB?"),  # overlapping zone between NationA and NationB – boundary may be ambiguous
        ((30, 30), "NationC"),
        ((22, 22), "Should be in NationB if not contested"),
        ((8, 26), "Possibly NationC")
    ]
    
    for point, description in test_points:
        owner = crm.territory_owner(point)
        logging.info(f"Point {point} ({description}) is controlled by: {owner}")
    
    # Simulate a unit movement attempt:
    unit_nation = "NationA"
    target_point = (19, 19)
    can_enter = crm.can_unit_enter(unit_nation, target_point, is_capturing=True)
    logging.info(f"Unit from {unit_nation} attempting to capture at {target_point}: {'Allowed' if can_enter else 'Denied'}")
    
    # If allowed, simulate border movement (capture effect).
    if can_enter:
        crm.simulate_border_movement(unit_nation, target_point)
    
    # Test allied movement: unit from NationA trying to enter NationC territory.
    unit_nation = "NationA"
    target_point = (8, 30)
    can_enter = crm.can_unit_enter(unit_nation, target_point, is_capturing=True)
    logging.info(f"Unit from {unit_nation} attempting to capture allied territory at {target_point}: {'Allowed' if can_enter else 'Denied'}")
    
    # However, allow movement without capturing.
    can_enter = crm.can_unit_enter(unit_nation, target_point, is_capturing=False)
    logging.info(f"Unit from {unit_nation} moving through allied territory at {target_point} (no capture): {'Allowed' if can_enter else 'Denied'}")
    
    logging.info("CountryRelations simulation complete.")
