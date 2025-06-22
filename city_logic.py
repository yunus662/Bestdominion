#!/usr/bin/env python3
"""
city_logic.py
=============
Production‑Quality City Logic Module for Conqueror Engine

This module manages core operations on city data including:
  • Loading city data from a JSON file.
  • Applying per‑tier rules based on country codes.
  • Overriding or customizing details for specific cities.
  • Updating a city’s owner and upgrade properties.
  • Exposing enriched city data, including a panel data hook for UI display.

The module loads city data from './data/cities.json'. In production, you might load
this from a database or external API. Logging is used throughout for traceability.
"""

import json
import logging
import os

# Configure logging
logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s] %(message)s")

# ---------------------------
# Global Collections
# ---------------------------

# City tier rules based on country codes.
CITY_RULES = {
    "rich": {
        "countries": [
            "US", "DE", "JP", "GB", "FR", "CA", "AU", "KR", "NL", "SE", "CH", 
            "NO", "FI", "AT", "BE", "IE", "SG", "IS", "LU", "DK", "NZ", "AE", 
            "QA", "SA", "IL", "TW", "CZ", "SI", "EE", "LT", "LV", "MT", "CY"
        ],
        "rules": {
            "infrastructureLevel": 4,
            "hasRealRoads": True,
            "roadDensity": 4,
            "economicMultiplier": 1.6
        }
    },
    "middle": {
        "countries": [
            "CN", "IN", "BR", "MX", "ID", "TR", "RU", "TH", "MY", "PH", "PL", 
            "AR", "CL", "CO", "PE", "KZ", "EG", "ZA", "DZ", "IR", "MA", "VN", 
            "RO", "UA", "SR", "GE", "AM", "AZ", "BO", "EC", "UY", "TN", "LB", 
            "JO", "AL", "RS", "BA", "ME", "MK", "MD", "GH", "NG", "KE", "TZ", 
            "PK", "BD", "LK", "MM", "KH", "LA", "MN", "FJ", "PG", "VU", "SB", 
            "NA", "BW", "ZM", "ZW", "GN", "GA", "BJ", "CM", "SN", "ML", "BF", 
            "TD", "NE", "CI", "RW", "MG", "LS", "SZ", "GT", "HN", "SV", "NI", 
            "PA", "DO", "PY", "GY", "TT", "JM", "MU", "CV", "PS", "BI"
        ],
        "rules": {
            "infrastructureLevel": 2,
            "hasRealRoads": True,
            "roadDensity": 2,
            "economicMultiplier": 1.2
        }
    },
    "poor": {
        "countries": [
            "AF", "HT", "YE", "SD", "SS", "CD", "CF", "SO", "ER", "SL", "LR", 
            "GM", "MR", "DJ", "MW", "MZ", "UG", "TG", "GW", "KM", "NP", "BT", 
            "CG", "GQ", "TO", "TV", "KI", "FM", "MH", "NR", "ST", "TL", "MG", 
            "ET", "NG", "ML", "NE"
        ],
        "rules": {
            "infrastructureLevel": 1,
            "hasRealRoads": False,
            "roadDensity": 1,
            "economicMultiplier": 0.8
        }
    }
}

# Per‑city customizations: overrides for owner and upgrades.
CITY_OVERRIDES = {
    "Berlin": {
        "owner": "Germany",
        "upgrades": {"infrastructureLevel": 5, "roadDensity": 5, "economicMultiplier": 2.0}
    },
    "Mumbai": {
        "upgrades": {"infrastructureLevel": 3, "roadDensity": 3, "economicMultiplier": 1.4}
    },
    "Addis Ababa": {
        "upgrades": {"infrastructureLevel": 2, "roadDensity": 2, "economicMultiplier": 1.1}
    },
    "São Paulo": {
        "upgrades": {"infrastructureLevel": 4, "roadDensity": 3, "economicMultiplier": 1.6}
    },
    "Cairo": {
        "owner": "Egypt",
        "upgrades": {"infrastructureLevel": 3, "roadDensity": 2, "economicMultiplier": 1.3}
    }
}

# Global variable that will hold the loaded cities data.
CITIES = []  # This will be a list of country dictionaries as loaded from the JSON file.

# ---------------------------
# Data Loading
# ---------------------------

def load_cities_data(file_path="./data/cities.json"):
    """
    Loads cities data from a JSON file.
    
    Args:
        file_path (str): Path to the JSON file containing city data.
    
    Returns:
        list: Parsed JSON data as a list of country dictionaries.
    """
    if not os.path.exists(file_path):
        logging.error(f"Cities file '{file_path}' not found.")
        return []
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            data = json.load(f)
        logging.info(f"Loaded cities data from {file_path}")
        return data
    except Exception as e:
        logging.error(f"Error loading cities data: {e}")
        return []

# Load the cities data on module import.
CITIES = load_cities_data()

# ---------------------------
# Helper Functions
# ---------------------------

def get_rules_for_code(code):
    """
    Determines the base city rules for the provided country code based on tier definitions.
    
    Args:
        code (str): The ISO country code.
    
    Returns:
        dict: A dictionary of default rules.
    """
    for tier in CITY_RULES.values():
        if code in tier["countries"]:
            logging.debug(f"Country code {code} matched rules: {tier['rules']}")
            return tier["rules"]
    default = {"infrastructureLevel": 1, "hasRealRoads": False, "roadDensity": 1, "economicMultiplier": 1.0}
    logging.debug(f"Country code {code} did not match any tier; using default: {default}")
    return default

def get_owner(city_name, fallback):
    """
    Retrieves the owner override for a city if one exists, otherwise returns the fallback.
    
    Args:
        city_name (str): The name of the city.
        fallback (str): The default owner (usually the country's name).
    
    Returns:
        str: The owner for the city.
    """
    owner = CITY_OVERRIDES.get(city_name, {}).get("owner", fallback)
    logging.debug(f"Owner for '{city_name}': {owner} (fallback was '{fallback}')")
    return owner

def get_upgrade(city_name):
    """
    Retrieves any upgrade overrides for the city.
    
    Args:
        city_name (str): The city name.
    
    Returns:
        dict or None: Upgrade values if available, else None.
    """
    upgrades = CITY_OVERRIDES.get(city_name, {}).get("upgrades", None)
    logging.debug(f"Upgrades for '{city_name}': {upgrades}")
    return upgrades

# ---------------------------
# Dynamic API Functions
# ---------------------------

def set_city_owner(city_name, new_owner):
    """
    Sets (or overrides) the owner of a specified city in the city overrides.
    
    Args:
        city_name (str): The name of the city.
        new_owner (str): The new owner to assign.
    """
    if city_name not in CITY_OVERRIDES:
        CITY_OVERRIDES[city_name] = {}
    CITY_OVERRIDES[city_name]["owner"] = new_owner
    logging.info(f"Set owner for '{city_name}' to '{new_owner}'.")

def set_city_upgrades(city_name, upgrades):
    """
    Sets or updates upgrade values for a specified city.
    
    Args:
        city_name (str): The city name.
        upgrades (dict): A dictionary of upgrade values (e.g., infrastructureLevel, roadDensity, economicMultiplier).
    """
    if city_name not in CITY_OVERRIDES:
        CITY_OVERRIDES[city_name] = {}
    CITY_OVERRIDES[city_name]["upgrades"] = upgrades
    logging.info(f"Set upgrades for '{city_name}' to {upgrades}.")

def get_cities_by_owner(nation):
    """
    Returns a list of all cities owned by the specified nation.
    
    Args:
        nation (str): The nation name.
    
    Returns:
        list: A list of city dictionaries.
    """
    enriched = get_cities_with_rules()
    # Flatten the list: each country dictionary has a 'cities' list.
    all_cities = [city for country in enriched for city in country.get("cities", [])]
    owned = [city for city in all_cities if city.get("owner") == nation]
    logging.debug(f"Found {len(owned)} cities owned by {nation}.")
    return owned

# ---------------------------
# Main Data Enrichment Function
# ---------------------------

def get_cities_with_rules():
    """
    Returns fully enriched city data. For each country in the loaded CITIES, the function looks up base
    rules based on the country code, and then for each city in that country, applies:
      - The base rules.
      - Any upgrade overrides.
      - Sets the city owner (from overrides or defaults to the country's name).
      - Adds a key "nation" that is the country's name.
    
    Returns:
        list: A list of enriched country dictionaries.
    """
    enriched_countries = []
    for country in CITIES:
        rules = get_rules_for_code(country.get("code"))
        country_name = country.get("name", "Unknown")
        enriched_cities = []
        for city in country.get("cities", []):
            upgrades = get_upgrade(city.get("name"))
            owner = get_owner(city.get("name"), country_name)
            enriched_city = {**city, **rules}
            if upgrades:
                enriched_city.update(upgrades)
            enriched_city["owner"] = owner
            enriched_city["nation"] = country_name
            enriched_cities.append(enriched_city)
        enriched_country = {**country, "cities": enriched_cities}
        enriched_countries.append(enriched_country)
    logging.info(f"Enriched cities data prepared for {len(enriched_countries)} countries.")
    return enriched_countries

def get_city_panel_data(city_name):
    """
    Retrieves panel data (i.e. full enriched city data) for the specified city name.
    
    Args:
        city_name (str): The city to look up.
    
    Returns:
        dict or None: The enriched city dictionary if found, else None.
    """
    enriched = get_cities_with_rules()
    for country in enriched:
        for city in country.get("cities", []):
            if city.get("name") == city_name:
                logging.info(f"Panel data found for city '{city_name}'.")
                return city
    logging.warning(f"City panel data for '{city_name}' not found.")
    return None

# ---------------------------
# Standalone Testing Block
# ---------------------------
if __name__ == "__main__":
    logging.info("=== Standalone Test for city_logic.py ===")

    # Retrieve enriched countries and their cities.
    countries = get_cities_with_rules()
    logging.info(f"Loaded {len(countries)} countries from data.")

    # Example: print all cities for each country.
    for country in countries:
        logging.info(f"Country: {country.get('name')} - Cities: {[city.get('name') for city in country.get('cities', [])]}")

    # Test dynamic API: Update owner for a city.
    set_city_owner("Berlin", "Germany")
    # Test dynamic API: Update upgrades for a city.
    set_city_upgrades("Mumbai", {"infrastructureLevel": 3, "roadDensity": 3, "economicMultiplier": 1.4})

    # Get cities by owner: Example
    berlin_cities = get_cities_by_owner("Germany")
    logging.info(f"Cities owned by Germany: {[city.get('name') for city in berlin_cities]}")

    # Retrieve panel data for a specific city.
    panel_data = get_city_panel_data("Cairo")
    if panel_data:
        logging.info(f"City panel data for Cairo: {panel_data}")
    else:
        logging.info("No panel data found for Cairo.")

    logging.info("Standalone test for city_logic.py complete.")
