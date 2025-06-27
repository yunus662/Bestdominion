/**************************************************************************************************
 * buildings.h
 * Production-Quality Buildings Module for Conqueror Engine (Header)
 *
 * Version: 2.0
 * Last Updated: 2025-06-27
 *
 * This header file defines the interfaces for the building system in the Conqueror Engine.
 * It declares the data structures, classes, and functions related to building management,
 * allowing other modules to interact with the building system.
 *
 * Contents:
 * - BuildingVariant: A struct holding the detailed properties for different types of buildings.
 * - Building (and derived classes): The core classes representing individual buildings in the game.
 * - BuildingManager: A class to orchestrate the purchase, upgrade, and production simulation
 * of all buildings belonging to a nation.
 *
 * To use this module, include this header file in your .cpp files and link against the
 * compiled buildings.cpp object file.
 **************************************************************************************************/

#ifndef BUILDINGS_H
#define BUILDINGS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>

// Forward declaration for the manager class to use.
class Building;

//-------------------------------------------------
// Data Structure for Building Variants
//-------------------------------------------------
struct BuildingVariant {
    std::string category;           // e.g., "Barracks", "Factory"
    std::string variantName;        // Specific name, e.g., "M1 Infantry Barracks"
    double cost;                    // Cost to purchase
    double upgradeCost;             // Base cost for upgrades
    double buildTime;               // Time to construct in minutes
    double productionBonus;         // Production multiplier (if applicable)
    bool subscriptionRequired;      // Flag for premium content
    std::string iconPath;           // Path to UI icon
};

//-------------------------------------------------
// Global Data and Functions
//-------------------------------------------------

// Global registry mapping a building category to its list of variants.
// This is declared here as 'extern' to indicate it's defined in a .cpp file.
extern std::map<std::string, std::vector<BuildingVariant>> g_buildingVariants;

/**
 * @brief Initializes the global g_buildingVariants registry with data.
 * Must be called once at application startup.
 */
void initBuildingVariants();

//-------------------------------------------------
// Base Building Class Definition
//-------------------------------------------------
class Building {
public:
    // Public attributes for easy access within the engine
    std::string category;
    BuildingVariant variant;
    int level;
    double health;

    /**
     * @brief Construct a new Building object.
     * @param cat The category of the building (e.g., "Barracks").
     * @param var The specific variant data for this building.
     */
    Building(const std::string& cat, const BuildingVariant& var);

    // Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~Building() = default;

    /**
     * @brief Upgrades the building to the next level, enhancing its stats.
     */
    virtual void upgrade();

    /**
     * @brief Simulates resource production for one game tick.
     * @return The amount of resources produced. Defaults to 0.
     */
    virtual double produce();

    /**
     * @brief Gets a formatted string with information about the building.
     * @return A string containing the building's details.
     */
    virtual std::string getInfo() const;
};

//-------------------------------------------------
// Derived Building Class: ResourceMine
//-------------------------------------------------
class ResourceMine : public Building {
public:
    /**
     * @brief Construct a new ResourceMine object.
     * @param var The variant data for this mine.
     */
    explicit ResourceMine(const BuildingVariant& var);

    /**
     * @brief Calculates resource production based on level and variant bonus.
     * @return The amount of resources produced this tick.
     */
    double produce() override;

    /**
     * @brief Gets enhanced information string including production details.
     * @return A string with the mine's details.
     */
    std::string getInfo() const override;
};

// TODO: Add other derived building classes here (e.g., Barracks for unit training,
// Factory for equipment production) following the ResourceMine pattern.


//-------------------------------------------------
// BuildingManager Class Definition
//-------------------------------------------------
class BuildingManager {
private:
    std::vector<std::unique_ptr<Building>> buildings; // Owns all building objects for a nation.
    mutable std::mutex managerMutex; // Protects the buildings vector from concurrent access.

public:
    BuildingManager();

    /**
     * @brief Purchases a new building for the nation.
     * @param buildingCategory The category of building to buy (e.g., "Factory").
     * @param posX The X coordinate for the building's location.
     * @param posY The Y coordinate for the building's location.
     * @param nationName The name of the owning nation (for logging).
     * @param nationTreasury A reference to the nation's treasury, which will be debited.
     * @return A raw pointer to the newly created building, or nullptr if the purchase fails.
     * The BuildingManager retains ownership of the object.
     */
    Building* buyBuilding(const std::string& buildingCategory, double posX, double posY, const std::string& nationName, double& nationTreasury);

    /**
     * @brief Upgrades an existing building.
     * @param index The index of the building in the manager's list.
     * @param nationTreasury A reference to the nation's treasury, which will be debited.
     * @return True if the upgrade was successful, false otherwise.
     */
    bool upgradeBuilding(int index, double& nationTreasury);

    /**
     * @brief Simulates production for all applicable buildings managed by this instance.
     * @return The total amount of resources produced by all buildings this tick.
     */
    double simulateProduction();

    /**
     * @brief Dumps the status of all managed buildings to the log.
     */
    void dumpBuildings() const;
};

#endif // BUILDINGS_H
