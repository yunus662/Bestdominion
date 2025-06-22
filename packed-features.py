#!/usr/bin/env python3
"""
packed_features.py
==================
Production‑Quality Packed Features Module for Conqueror Engine (Python Version)

This module consolidates core nation‑level gameplay features including:
  • Initialization of the country system from enriched city data
  • Estimation of economic indicators (GDP, treasury, daily income)
  • Nation operations:
      - Upgrading city infrastructure (with cost deductions)
      - Declaring war, making peace, and conquering cities
      - Displaying a detailed nation portfolio

Dependencies:
  - Functions from city_logic.py:
        get_cities_with_rules, set_city_owner, set_city_upgrades, get_city_panel_data
  - A stub full_country_data list for GDP and population data.
  
In production, replace dummy data with real sources and integrate with your UI.
"""

import logging
import threading
import time

from city_logic import (
    get_cities_with_rules,
    set_city_owner,
    set_city_upgrades,
    get_city_panel_data,
)

# Configure logging
logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s] %(message)s")

# ----- Dummy Full Country Data -----
# In production, import this from a dedicated data source.
full_country_data = [
    {"name": "TestLand", "gdp": 300, "population": 1_500_000},
    {"name": "Neighboria", "gdp": 250, "population": 2_000_000},
    {"name": "Germany", "gdp": 400, "population": 80_000_000},
    {"name": "Egypt", "gdp": 200, "population": 100_000_000},
]

# ----- Global Variables -----
current_nation = None  # The nation controlled by the player.
nations = {}         # Nation registry keyed by nation name.

# ----- Economy Estimation Functions -----

def estimate_income(gdp):
    """Estimate daily income based on GDP."""
    if gdp > 10000:
        return 20_000_000
    if gdp > 1000:
        return 5_000_000
    if gdp > 100:
        return 1_000_000
    return 100_000

def estimate_starting_treasury(gdp):
    """Estimate starting treasury based on GDP."""
    if gdp > 10000:
        return 1_000_000_000
    if gdp > 1000:
        return 300_000_000
    if gdp > 100:
        return 80_000_000
    return 20_000_000

# ----- Initialization and Nation Menu -----

def init_country_system():
    """
    Initializes the country system using enriched city data.
    
    Prompts the user to select which nation to lead. Then constructs a nation registry by
    iterating over the enriched city data and matching it with GDP/population from full_country_data.
    It also starts income accumulation for the chosen nation.
    """
    global current_nation, nations
    
    enriched_countries = get_cities_with_rules()  # List of country dictionaries
    print("Available Nations:")
    for country in enriched_countries:
        print(f"- {country.get('name')}")
        
    chosen = input("Enter the name of the nation you wish to lead: ").strip()
    current_nation = chosen
    logging.info(f"You are now leading {chosen}")
    
    # Build nation registry.
    for country in enriched_countries:
        nation_name = country.get("name")
        stub = next((c for c in full_country_data if c["name"] == nation_name), {})
        gdp = stub.get("gdp", 100)
        pop = stub.get("population", 1_000_000)
        nations[nation_name] = {
            "name": nation_name,
            "gdp": gdp,
            "population": pop,
            "treasury": estimate_starting_treasury(gdp),
            "daily_income": estimate_income(gdp),
            "infra_level": 1,
            "at_war_with": [],
            "cities": [city["name"] for city in country.get("cities", [])],
            "units": []
        }
    logging.info(f"Nation data initialized: {nations}")
    
    # Start income accumulation in a background thread.
    threading.Thread(target=accumulate_income, daemon=True).start()
    
    # Display the portfolio for the current nation.
    show_nation_portfolio(current_nation)

def accumulate_income():
    """
    Runs continuously to add income to the current nation's treasury.
    For simulation purposes, income is added every 10 seconds.
    """
    global current_nation, nations
    while True:
        if current_nation and current_nation in nations:
            nation = nations[current_nation]
            nation["treasury"] += nation["daily_income"] * nation["infra_level"]
            logging.info(f"Income added to {current_nation}. Treasury now: ${nation['treasury']:,}")
        time.sleep(10)

# ----- Nation Operations -----

def upgrade_infrastructure(city_name):
    """
    Upgrades the infrastructure of a specified city if it belongs to the current nation.
    
    Checks city ownership and adjusts treasury based on current city infrastructure level.
    Updates the city's upgrades via set_city_upgrades().
    """
    owner = get_city_owner(city_name)
    if owner != current_nation:
        logging.warning(f"You can't upgrade {city_name}; it's not under your control.")
        return
    nation = nations[current_nation]
    city_level = get_city_infra_level(city_name)
    cost = city_level * 500_000
    if nation["treasury"] < cost:
        logging.warning(f"Not enough treasury to upgrade {city_name}. Required: ${cost:,}")
        return
    nation["treasury"] -= cost
    next_level = min(city_level + 1, 5)
    new_multiplier = 1 + next_level * 0.2
    set_city_upgrades(city_name, {
        "infrastructureLevel": next_level,
        "roadDensity": next_level,
        "economicMultiplier": new_multiplier
    })
    logging.info(f"{city_name} upgraded to level {next_level}.")

def declare_war(target_name):
    """
    Declares war on a target nation by adding it to the current nation's at-war list.
    """
    nation = nations.get(current_nation)
    target = nations.get(target_name)
    if not nation or not target:
        logging.error("Invalid nation specified for war declaration.")
        return
    if target_name not in nation["at_war_with"]:
        nation["at_war_with"].append(target_name)
        logging.info(f"{nation['name']} declared war on {target_name}.")

def make_peace(target_name):
    """
    Makes peace with a target nation by removing it from the current nation's at-war list.
    """
    nation = nations.get(current_nation)
    if not nation:
        return
    nation["at_war_with"] = [n for n in nation["at_war_with"] if n != target_name]
    logging.info(f"{nation['name']} made peace with {target_name}.")

def conquer_city(city_name):
    """
    Conquers a city by transferring ownership to the current nation.
    Updates the city owner using set_city_owner() and adjusts the cities lists in nations.
    """
    prev_owner = get_city_owner(city_name)
    if prev_owner == current_nation:
        logging.info(f"{city_name} is already under your control.")
        return
    conquering = nations.get(current_nation)
    loser = nations.get(prev_owner)
    if not conquering or not loser:
        logging.error("Error retrieving nation data during conquest.")
        return
    loser["cities"] = [c for c in loser["cities"] if c != city_name]
    conquering["cities"].append(city_name)
    set_city_owner(city_name, current_nation)
    logging.info(f"{current_nation} has conquered {city_name} from {prev_owner}.")

def show_nation_portfolio(nation_name):
    """
    Displays a textual summary of a nation's portfolio including population, GDP, treasury,
    daily income, infrastructure level, and list of cities.
    
    Returns:
        str: The portfolio summary.
    """
    nation = nations.get(nation_name)
    if not nation:
        logging.error(f"Nation '{nation_name}' not found.")
        return ""
    portfolio = (
        f"==== {nation['name']} Portfolio ====\n"
        f"Population: {nation['population']:,}\n"
        f"GDP: ${nation['gdp']}B\n"
        f"Daily Income: ${nation['daily_income']:,}\n"
        f"Treasury: ${nation['treasury']:,}\n"
        f"Infrastructure Level: {nation['infra_level']}\n"
        f"Cities: {len(nation['cities'])}\n"
        "Cities: " + ", ".join(nation['cities']) + "\n"
        "============================="
    )
    logging.info(portfolio)
    return portfolio

def get_nation_data(name):
    """
    Retrieves the nation data for the specified nation.
    
    Args:
        name (str): The nation name.
    
    Returns:
        dict or None: The nation data if available.
    """
    return nations.get(name)

def get_all_cities():
    """
    Returns a list of all cities across nations, each represented as a dictionary with keys: name, nation, and lat/lng.
    """
    all_cities = []
    enriched = get_cities_with_rules()
    for country in enriched:
        nation_name = country.get("name")
        for city in country.get("cities", []):
            all_cities.append({
                "name": city.get("name"),
                "nation": nation_name,
                "latlng": (city.get("lat"), city.get("lng"))
            })
    return all_cities

# ----- Helper Functions -----

def get_city_owner(city_name):
    """
    Retrieves the owner of a city from the enriched city data.
    
    Args:
        city_name (str): Name of the city.
    
    Returns:
        str: The owner's name, or "Unknown" if not found.
    """
    enriched = get_cities_with_rules()
    for country in enriched:
        for city in country.get("cities", []):
            if city.get("name") == city_name:
                return city.get("owner", "Unknown")
    return "Unknown"

def get_city_infra_level(city_name):
    """
    Retrieves the infrastructure level of a city from the enriched data.
    
    Args:
        city_name (str): Name of the city.
    
    Returns:
        int: The infrastructure level, defaulting to 1.
    """
    enriched = get_cities_with_rules()
    for country in enriched:
        for city in country.get("cities", []):
            if city.get("name") == city_name:
                return city.get("infrastructureLevel", 1)
    return 1

# ----- Standalone Simulation -----

if __name__ == "__main__":
    logging.info("=== Starting Packed Features Simulation ===")
    
    # Initialize nation data via city information.
    init_country_system()
    
    # Brief delay to observe income accumulation.
    time.sleep(12)
    
    # Simulate various nation operations:
    upgrade_infrastructure("Alpha City")
    declare_war("Neighboria")
    conquer_city("Gamma Metropolis")
    make_peace("Neighboria")
    show_nation_portfolio(current_nation)
    
    # List all cities.
    all_cities = get_all_cities()
    logging.info(f"Total cities available: {len(all_cities)}")
    
    logging.info("=== Packed Features Simulation Complete ===")
