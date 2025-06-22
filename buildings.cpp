/********************************************************************************************************************
 * buildings.cpp
 * Production‑Quality Buildings Module for Conqueror Engine (C++ Version)
 *
 * This module implements a comprehensive building system for the Conqueror Engine.
 * It covers:
 *   - Definition of a BuildingVariant structure that stores the advanced variant details for each building type.
 *   - A global registry mapping each building category to a vector of 7 variant definitions.
 *   - A base Building class that supports attributes such as health, level, build cost, upgrade cost, and production
 *     bonuses.
 *   - A BuildingManager class to handle for a nation: purchasing buildings, upgrading them, and simulating resource
 *     production from buildings.
 *   - Extensive logging and inline documentation for production-quality error checking and debugging.
 *
 * In a full production system, advanced variants (the seventh variant for each type) will require tickets or a subscription.
 *
 * Build with (example):
 *   g++ buildings.cpp -o buildings -std=c++11 -pthread
 ********************************************************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <thread>
#include <chrono>

// -------------------------------------------------
// Logging utility (simulates logEvent from other modules)
void logEvent(const std::string &message, const std::string &level = "INFO") {
    std::cout << "[" << level << "] " << message << std::endl;
}

// -------------------------------------------------
// Data Structure for Building Variants
struct BuildingVariant {
    std::string category;         // e.g., "Barracks", "Factory", etc.
    std::string variantName;      // Real-life or advanced variant name.
    double cost;                  // Money cost to build.
    double upgradeCost;           // Base upgrade cost.
    double buildTime;             // Construction time in minutes.
    double productionBonus;       // Bonus multiplier for resource production, if applicable.
    bool subscriptionRequired;    // True if requires tickets/subscription.
    std::string iconPath;         // Path to the building icon.
};

// Global registry mapping building category to a vector of advanced variants.
std::map<std::string, std::vector<BuildingVariant>> g_buildingVariants;

// -------------------------------------------------
// Initialize the Building Variants for Each Category
void initBuildingVariants() {
    // ----- Barracks -----
    std::vector<BuildingVariant> barracks;
    barracks.push_back({"Barracks", "M1 Infantry Barracks", 200000, 50000, 10, 1.0, false, "icons/barracks_m1.png"});
    barracks.push_back({"Barracks", "Modular Infantry Base", 220000, 55000, 11, 1.05, false, "icons/barracks_modular.png"});
    barracks.push_back({"Barracks", "Rapid Deployment Battalion Base", 240000, 60000, 12, 1.10, false, "icons/barracks_rdb.png"});
    barracks.push_back({"Barracks", "Urban Warfare Training Center", 260000, 65000, 13, 1.15, false, "icons/barracks_urban.png"});
    barracks.push_back({"Barracks", "Tech-Enhanced Infantry Depot", 280000, 70000, 14, 1.20, false, "icons/barracks_tech.png"});
    barracks.push_back({"Barracks", "Modular Robotics Barracks", 300000, 75000, 15, 1.25, false, "icons/barracks_robotics.png"});
    barracks.push_back({"Barracks", "Next-Gen Elite Command Center", 500000, 100000, 20, 1.50, true, "icons/barracks_elite.png"});
    g_buildingVariants["Barracks"] = barracks;

    // ----- Factory -----
    std::vector<BuildingVariant> factories;
    factories.push_back({"Factory", "Conventional Arms Factory", 400000, 120000, 20, 1.0, false, "icons/factory_conventional.png"});
    factories.push_back({"Factory", "Automated Production Facility", 420000, 125000, 21, 1.05, false, "icons/factory_automated.png"});
    factories.push_back({"Factory", "Modular Assembly Plant", 440000, 130000, 22, 1.10, false, "icons/factory_modular.png"});
    factories.push_back({"Factory", "Precision Manufacturing Hub", 460000, 135000, 23, 1.15, false, "icons/factory_precision.png"});
    factories.push_back({"Factory", "High-Tech Robotic Foundry", 480000, 140000, 24, 1.20, false, "icons/factory_robotics.png"});
    factories.push_back({"Factory", "Integrated Production Network", 500000, 145000, 25, 1.25, false, "icons/factory_integrated.png"});
    factories.push_back({"Factory", "Quantum-Level Advanced Factory", 800000, 200000, 30, 1.50, true, "icons/factory_advanced.png"});
    g_buildingVariants["Factory"] = factories;

    // ----- Research Center -----
    std::vector<BuildingVariant> researchCenters;
    researchCenters.push_back({"Research Center", "Basic Military Research Lab", 300000, 80000, 15, 1.0, false, "icons/research_basic.png"});
    researchCenters.push_back({"Research Center", "Modernization Research Facility", 320000, 85000, 16, 1.05, false, "icons/research_modern.png"});
    researchCenters.push_back({"Research Center", "Advanced Experimental Research Lab", 340000, 90000, 17, 1.10, false, "icons/research_advanced.png"});
    researchCenters.push_back({"Research Center", "Integrated Systems Research Center", 360000, 95000, 18, 1.15, false, "icons/research_integrated.png"});
    researchCenters.push_back({"Research Center", "Next-Gen Theory and Application Lab", 380000, 100000, 19, 1.20, false, "icons/research_nextgen.png"});
    researchCenters.push_back({"Research Center", "Multidisciplinary Innovation Hub", 400000, 105000, 20, 1.25, false, "icons/research_innovation.png"});
    researchCenters.push_back({"Research Center", "Ultra-Advanced Strategic Research Center", 700000, 150000, 25, 1.50, true, "icons/research_ultra.png"});
    g_buildingVariants["Research Center"] = researchCenters;

    // ----- Defensive Tower -----
    std::vector<BuildingVariant> defTowers;
    defTowers.push_back({"Defensive Tower", "Basic Watchtower", 150000, 40000, 8, 0.0, false, "icons/tower_basic.png"});
    defTowers.push_back({"Defensive Tower", "Reinforced Guard Tower", 170000, 45000, 9, 0.0, false, "icons/tower_reinforced.png"});
    defTowers.push_back({"Defensive Tower", "Automated Defense Tower", 190000, 50000, 10, 0.0, false, "icons/tower_automated.png"});
    defTowers.push_back({"Defensive Tower", "Advanced Missile Defense Tower", 210000, 55000, 11, 0.0, false, "icons/tower_missile.png"});
    defTowers.push_back({"Defensive Tower", "Smart Sensor Defense Tower", 230000, 60000, 12, 0.0, false, "icons/tower_sensor.png"});
    defTowers.push_back({"Defensive Tower", "AI-Controlled Defense Tower", 250000, 65000, 13, 0.0, false, "icons/tower_ai.png"});
    defTowers.push_back({"Defensive Tower", "Next-Generation Laser Defense Tower", 450000, 90000, 18, 0.0, true, "icons/tower_laser.png"});
    g_buildingVariants["Defensive Tower"] = defTowers;

    // ----- Resource Mine -----
    std::vector<BuildingVariant> mines;
    mines.push_back({"Resource Mine", "Standard Gold Mine", 180000, 50000, 7, 1.0, false, "icons/mine_gold.png"});
    mines.push_back({"Resource Mine", "High-Yield Gold Mine", 200000, 55000, 8, 1.10, false, "icons/mine_highyield.png"});
    mines.push_back({"Resource Mine", "Automated Gold Extraction Plant", 220000, 60000, 9, 1.20, false, "icons/mine_automated.png"});
    mines.push_back({"Resource Mine", "Integrated Mineral Processing Facility", 240000, 65000, 10, 1.30, false, "icons/mine_integrated.png"});
    mines.push_back({"Resource Mine", "Ultra-Efficient Extractor", 260000, 70000, 11, 1.40, false, "icons/mine_ultra.png"});
    mines.push_back({"Resource Mine", "Smart Mining Operation", 280000, 75000, 12, 1.50, false, "icons/mine_smart.png"});
    mines.push_back({"Resource Mine", "Quantum-Enhanced Resource Extractor", 500000, 100000, 16, 2.0, true, "icons/mine_quantum.png"});
    g_buildingVariants["Resource Mine"] = mines;
}

// -------------------------------------------------
// Base Building Class
class Building {
public:
    std::string category;
    BuildingVariant variant;
    int level;
    double health;         // Health points of the building.
    
    // Constructors.
    Building(const std::string &cat, const BuildingVariant &var)
        : category(cat), variant(var), level(1), health(1000.0) {
        logEvent("Building created: " + variant.variantName, "DEBUG");
    }
    
    // Virtual destructor.
    virtual ~Building() {}
    
    // Upgrade the building, increasing level and modifying stats.
    virtual void upgrade() {
        level++;
        // Increase health and upgrade cost by a factor (for simplicity).
        health *= 1.2;
        logEvent("Upgraded " + variant.variantName + " to level " + std::to_string(level), "INFO");
    }
    
    // Simulate production; for Resource Mines or Factories, returns resource produced.
    virtual double produce() {
        // Default: no production.
        return 0.0;
    }
    
    // Display building info.
    virtual std::string getInfo() const {
        std::ostringstream oss;
        oss << "Building - Category: " << category 
            << ", Variant: " << variant.variantName 
            << ", Level: " << level 
            << ", Health: " << std::fixed << std::setprecision(2) << health;
        return oss.str();
    }
};

// -------------------------------------------------
// Derived Building: ResourceMine produces resources.
class ResourceMine : public Building {
public:
    // Production multiplier is based on variant.productionBonus.
    ResourceMine(const BuildingVariant &var)
        : Building("Resource Mine", var) {}
    
    virtual double produce() override {
        // Production = base production * bonus * level.
        double baseProduction = 100.0; 
        double production = baseProduction * variant.productionBonus * level;
        std::ostringstream oss;
        oss << variant.variantName << " produced " << production << " units.";
        logEvent(oss.str(), "DEBUG");
        return production;
    }
    
    virtual std::string getInfo() const override {
        std::ostringstream oss;
        oss << Building::getInfo() 
            << ", Production Bonus: " << variant.productionBonus;
        return oss.str();
    }
};

// Other derived classes (e.g., Barracks, Factory, ResearchCenter, DefensiveTower)
// can follow the same pattern if they produce units or provide other bonuses.

// -------------------------------------------------
// BuildingManager: Manages buildings for a nation.
class BuildingManager {
public:
    // For simplicity, we maintain a vector of buildings.
    std::vector<std::unique_ptr<Building>> buildings;
    std::mutex mtx;
    
    BuildingManager() {
        logEvent("BuildingManager initialized.", "DEBUG");
    }
    
    // Purchase a building: checks if nation can afford (nationTreasury modified externally), then creates it.
    // Returns pointer to the building or nullptr if purchase failed.
    Building* buyBuilding(const std::string &buildingCategory, double posX, double posY, const std::string &nationName, double &nationTreasury) {
        if (g_buildingVariants.find(buildingCategory) == g_buildingVariants.end()) {
            logEvent("Building category " + buildingCategory + " not found.", "ERROR");
            return nullptr;
        }
        auto &variants = g_buildingVariants[buildingCategory];
        if (variants.empty()) {
            logEvent("No variants available for " + buildingCategory, "ERROR");
            return nullptr;
        }
        // For demonstration, choose the cheapest variant.
        BuildingVariant chosenVariant = variants.front();
        if (nationTreasury < chosenVariant.cost) {
            logEvent(nationName + " cannot afford " + buildingCategory + " (" + chosenVariant.variantName + ").", "WARNING");
            return nullptr;
        }
        nationTreasury -= chosenVariant.cost;
        // Create building.
        std::unique_ptr<Building> building;
        if (buildingCategory == "Resource Mine") {
            building.reset(new ResourceMine(chosenVariant));
        } else {
            // For other building types, use the generic base Building.
            building.reset(new Building(buildingCategory, chosenVariant));
        }
        Building* ptr = building.get();
        {
            std::lock_guard<std::mutex> lock(mtx);
            buildings.push_back(std::move(building));
        }
        std::ostringstream oss;
        oss << nationName << " purchased " << buildingCategory << " (" << chosenVariant.variantName << ") for $"
            << std::fixed << std::setprecision(2) << chosenVariant.cost;
        logEvent(oss.str(), "INFO");
        return ptr;
    }
    
    // Upgrade a building identified by index.
    bool upgradeBuilding(int index, double &nationTreasury) {
        std::lock_guard<std::mutex> lock(mtx);
        if (index < 0 || index >= static_cast<int>(buildings.size())) {
            logEvent("Invalid building index for upgrade.", "ERROR");
            return false;
        }
        // Calculate upgrade cost as base upgrade cost * current level.
        double cost = buildings[index]->variant.upgradeCost * buildings[index]->level;
        if (nationTreasury < cost) {
            std::ostringstream oss;
            oss << "Insufficient treasury to upgrade building. Required: $" 
                << std::fixed << std::setprecision(2) << cost;
            logEvent(oss.str(), "WARNING");
            return false;
        }
        nationTreasury -= cost;
        buildings[index]->upgrade();
        return true;
    }
    
    // Simulate production from all resource producing buildings.
    double simulateProduction() {
        double totalProduction = 0.0;
        std::lock_guard<std::mutex> lock(mtx);
        for (auto &b : buildings) {
            totalProduction += b->produce();
        }
        return totalProduction;
    }
    
    // Dump details of all buildings.
    void dumpBuildings() {
        std::lock_guard<std::mutex> lock(mtx);
        for (size_t i = 0; i < buildings.size(); ++i) {
            logEvent("Building[" + std::to_string(i) + "]: " + buildings[i]->getInfo(), "DEBUG");
        }
    }
};

// -------------------------------------------------
// Standalone testing block for buildings module
#ifdef BUILDINGS_TEST
int main() {
    // Seed building variants.
    initBuildingVariants();
    
    // Simulated nation treasury.
    double nationTreasury = 5000000;  // 5 million dollars.
    logEvent("Nation treasury initially: $" + std::to_string(nationTreasury));
    
    // Create the BuildingManager.
    BuildingManager manager;
    
    // Purchase several buildings.
    Building* b1 = manager.buyBuilding("Barracks", 100.0, 200.0, "TestNation", nationTreasury);
    Building* b2 = manager.buyBuilding("Factory", 150.0, 250.0, "TestNation", nationTreasury);
    Building* b3 = manager.buyBuilding("Resource Mine", 200.0, 300.0, "TestNation", nationTreasury);
    Building* b4 = manager.buyBuilding("Research Center", 250.0, 350.0, "TestNation", nationTreasury);
    Building* b5 = manager.buyBuilding("Defensive Tower", 300.0, 400.0, "TestNation", nationTreasury);
    
    logEvent("Nation treasury after purchases: $" + std::to_string(nationTreasury));
    
    manager.dumpBuildings();
    
    // Upgrade the first building.
    if (manager.upgradeBuilding(0, nationTreasury)) {
        logEvent("Upgraded building 0 successfully.");
    }
    logEvent("Nation treasury after upgrade: $" + std::to_string(nationTreasury));
    
    // Simulate production.
    double production = manager.simulateProduction();
    std::ostringstream oss;
    oss << "Total production from resource buildings: " << production << " units.";
    logEvent(oss.str(), "INFO");
    
    return 0;
}
#endif

/********************************************************************************************************************
 * End of buildings.cpp
 ********************************************************************************************************************/

