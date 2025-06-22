#!/usr/bin/env python3
"""
cities_model.py
===============
Production‑Quality City Simulation Module for Conqueror Engine

This module implements a simulation model for cities inspired by global strategy games
(such as Conflict of Nations). It provides detailed representations of urban centers 
through their economic, demographic, and infrastructural attributes.

Key Features:
  • City class:
      - Attributes: name, population, infrastructure level, economic index, defense index, 
        growth rate, and location (latitude and longitude).
      - Methods: simulate_growth, update_economy, improve_infrastructure, update_defense.
  • CityManager class:
      - Manages a collection of cities.
      - Methods: add_city, update_all (simulate a time-step), get_city_status, and dump_summary.
  • Embedded logging and detailed inline documentation for production‑level quality and future extension.
  
Usage:
  Import this module into your Conqueror Engine to simulate urban dynamics. Running this 
  module standalone will execute a sample simulation loop.
"""

import logging
import random
import time
from datetime import datetime, timedelta

# -------------------------------------------------------------------------
# Logging Configuration
# -------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(asctime)s] [%(levelname)s] %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# -------------------------------------------------------------------------
# City Class
# -------------------------------------------------------------------------
class City:
    """
    A class representing a city in the simulation.

    Attributes:
      name (str): Name of the city.
      population (int): The number of inhabitants.
      infrastructure (int): Level of infrastructure (1 to 10). Higher levels boost growth and economic output.
      economic_index (float): A multiplier (base 1.0) affecting resource production and revenue.
      defense_index (float): A rating (base 1.0) that influences resilience to crises or warfare.
      growth_rate (float): Annual percentage growth rate (e.g., 0.02 for 2% per simulated year).
      location (tuple): Geographic coordinates as (latitude, longitude).
      last_update (datetime): Timestamp of the last simulation update.
    """

    def __init__(self, name: str, population: int, infrastructure: int,
                 economic_index: float, defense_index: float,
                 growth_rate: float, location: tuple):
        self.name = name
        self.population = population
        self.infrastructure = infrastructure
        self.economic_index = economic_index
        self.defense_index = defense_index
        self.growth_rate = growth_rate  # Base annual growth rate.
        self.location = location  # (lat, lng)
        self.last_update = datetime.now()
        logging.debug(f"City created: {self}")

    def __str__(self):
        return (f"{self.name} | Pop: {self.population:,} | Infra: {self.infrastructure} | "
                f"Econ: {self.economic_index:.2f} | Def: {self.defense_index:.2f}")

    def simulate_growth(self, years: float = 1.0):
        """
        Simulate population growth over a given number of years.
        The growth is influenced by the base growth rate and infrastructure.
        
        Args:
            years (float): Number of years to simulate.
        """
        # A higher infrastructure level boosts the effective growth.
        effective_growth = self.growth_rate * (1 + (self.infrastructure / 20))
        old_pop = self.population
        self.population = int(self.population * ((1 + effective_growth) ** years))
        self.last_update = datetime.now()
        logging.info(f"{self.name} population grew from {old_pop:,} to {self.population:,} over {years:.2f} year(s).")

    def update_economy(self):
        """
        Update the city's economic output.
        Higher population and infrastructure lead to higher economic production,
        modulated by the economic index.
        Returns:
            production (float): The simulated economic production (e.g., revenue per year).
        """
        # Base production is proportional to population; infrastructure multiplies this effect.
        base_production = self.population * 0.01  # Example: 1% of population as base revenue.
        bonus = (self.infrastructure / 10) * self.economic_index
        production = base_production * (1 + bonus)
        logging.debug(f"{self.name} economic production updated: {production:.2f} units.")
        return production

    def improve_infrastructure(self, investment: float):
        """
        Improve the city's infrastructure by investing in upgrades.
        The investment increases the infrastructure level if sufficient funds are provided.
        
        Args:
            investment (float): The amount of money invested.
        
        Returns:
            success (bool): True if the upgrade was successful.
        """
        # Define a base upgrade cost that scales with current infrastructure level.
        base_cost = 100000 * (self.infrastructure)
        if investment >= base_cost:
            old_infra = self.infrastructure
            # Upgrade increases the level by 1 (cap at 10).
            if self.infrastructure < 10:
                self.infrastructure += 1
                # Improve economic and defense indices slightly.
                self.economic_index *= 1.05
                self.defense_index *= 1.05
                logging.info(f"{self.name} infrastructure upgraded from {old_infra} to {self.infrastructure}.")
                return True
            else:
                logging.info(f"{self.name} infrastructure is already at maximum level.")
                return False
        else:
            logging.warning(f"{self.name} investment of {investment} insufficient for upgrade (requires {base_cost}).")
            return False

    def update_defense(self, event_modifier: float):
        """
        Update the city's defense index sporadically. Some events may degrade or
        stimulate improvements in defense.
        
        Args:
            event_modifier (float): A multiplier effect (e.g., -0.1 reduces defense by 10%).
        """
        old_def = self.defense_index
        self.defense_index *= (1 + event_modifier)
        logging.debug(f"{self.name} defense index changed from {old_def:.2f} to {self.defense_index:.2f}.")

    def get_status(self):
        """
        Returns a dictionary summarizing the city's key statistics.
        """
        return {
            "name": self.name,
            "population": self.population,
            "infrastructure": self.infrastructure,
            "economic_index": self.economic_index,
            "defense_index": self.defense_index,
            "location": self.location,
            "last_update": self.last_update.isoformat()
        }

# -------------------------------------------------------------------------
# CityManager Class
# -------------------------------------------------------------------------
class CityManager:
    """
    Manages a collection of City objects and simulates their evolution over time.
    """
    def __init__(self):
        self.cities = {}  # Maps city names to City objects.
        logging.info("CityManager initialized.")

    def add_city(self, city: City):
        """
        Adds a new city to the management system.
        """
        if city.name in self.cities:
            logging.warning(f"City {city.name} already exists in the system.")
            return False
        self.cities[city.name] = city
        logging.info(f"City {city.name} added to the system.")
        return True

    def remove_city(self, city_name: str):
        """
        Removes a city by name.
        """
        if city_name in self.cities:
            del self.cities[city_name]
            logging.info(f"City {city_name} removed from the system.")
            return True
        logging.warning(f"City {city_name} not found in the system.")
        return False

    def update_all(self, years: float = 1.0):
        """
        Updates all cities in the system by simulating growth and economic activity.
        Args:
            years (float): Number of years to simulate.
        """
        logging.info("Updating all cities...")
        for city in self.cities.values():
            city.simulate_growth(years)
            production = city.update_economy()
            logging.debug(f"City {city.name} produced an economic output of {production:.2f} units.")
        logging.info("All cities updated.")

    def get_city_status(self, city_name: str):
        """
        Retrieves the status dictionary for the specified city.
        """
        if city_name in self.cities:
            status = self.cities[city_name].get_status()
            logging.debug(f"Status for {city_name}: {status}")
            return status
        logging.error(f"City {city_name} not found.")
        return None

    def dump_summary(self):
        """
        Prints a summary of all managed cities.
        """
        logging.info("------ City Summary ------")
        for city in self.cities.values():
            logging.info(str(city))
        logging.info("--------------------------")

# -------------------------------------------------------------------------
# Standalone Simulation
# -------------------------------------------------------------------------
if __name__ == '__main__':
    logging.info("Starting standalone simulation for cities_model.py")
    
    # Create a CityManager instance.
    manager = CityManager()
    
    # Add sample cities.
    city1 = City("Metropolis", population=1000000, infrastructure=5, economic_index=1.0,
                 defense_index=1.0, growth_rate=0.03, location=(40.7128, -74.0060))
    city2 = City("Gotham", population=750000, infrastructure=4, economic_index=0.95,
                 defense_index=1.1, growth_rate=0.025, location=(34.0522, -118.2437))
    city3 = City("Star City", population=500000, infrastructure=3, economic_index=1.1,
                 defense_index=0.9, growth_rate=0.035, location=(41.8781, -87.6298))
    
    manager.add_city(city1)
    manager.add_city(city2)
    manager.add_city(city3)
    
    # Dump initial status.
    manager.dump_summary()
    
    # Simulate a 1-year update cycle.
    logging.info("Simulating 1-year growth cycle...")
    manager.update_all(years=1.0)
    manager.dump_summary()
    
    # Process an infrastructure upgrade for one city.
    logging.info("Attempting to improve infrastructure of Gotham with a $500,000 investment...")
    success = city2.improve_infrastructure(500000)
    if success:
        logging.info("Infrastructure improved successfully.")
    else:
        logging.info("Infrastructure upgrade failed due to insufficient investment.")
    
    # Final status.
    manager.dump_summary()
    
    logging.info("City simulation complete.")
