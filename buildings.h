/**************************************************************************************************
 * buildings.h
 * Production-Quality Buildings Module for Conqueror Engine (Header)
 *
 * Version: 2.1
 * Last Updated: 2025-06-27
 *
 * This header defines the building system's interface used by the Conqueror Engine.
 * It includes building variant definitions, abstract and concrete building types, and
 * the manager class for coordinating construction and production logic.
 *
 * Exposed Classes:
 * - BuildingVariant
 * - Building (abstract base)
 * - ResourceMine (derived)
 * - BuildingManager
 **************************************************************************************************/

#ifndef BUILDINGS_H
#define BUILDINGS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

//-------------------------------------------------
// Building Variant Structure
//-------------------------------------------------
struct BuildingVariant {
    std::string category;              // e.g., "Barracks", "Factory"
    std::string variantName;           // Specific name, e.g., "M1 Infantry Barracks"
    double cost;                       // Initial construction cost
    double upgradeCost;                // Base cost for upgrades
    double buildTime;                  // Construction time (in minutes)
    double productionBonus;            // Resource or efficiency multiplier
    bool subscriptionRequired;         // True if requires tickets/subscription
    std::string iconPath;              // UI icon reference path
};

//-------------------------------------------------
// Global Registry and Initialization
//-------------------------------------------------
extern std::map<std::string, std::vector<BuildingVariant>> g_buildingVariants;

/**
 * @brief Populates the building variant registry. Call at engine startup.
 */
void initBuildingVariants();

//-------------------------------------------------
// Abstract Building Class
//-------------------------------------------------
class Building {
public:
    std::string category;
    BuildingVariant variant;
    int level;
    double health;

    Building(const std::string& cat, const BuildingVariant& var);
    virtual ~Building() = default;

    virtual void upgrade();
    virtual double produce();
    virtual std::string getInfo() const;
};

//-------------------------------------------------
// ResourceMine (Concrete Derived Class)
//-------------------------------------------------
class ResourceMine : public Building {
public:
    explicit ResourceMine(const BuildingVariant& var);

    double produce() override;
    std::string getInfo() const override;
};

//-------------------------------------------------
// Building Manager
//-------------------------------------------------
class BuildingManager {
private:
    std::vector<std::unique_ptr<Building>> buildings;
    mutable std::mutex managerMutex;

public:
    BuildingManager();

    Building* buyBuilding(const std::string& buildingCategory, double posX, double posY,
                          const std::string& nationName, double& nationTreasury);

    bool upgradeBuilding(int index, double& nationTreasury);
    double simulateProduction();
    void dumpBuildings() const;
};

#endif // BUILDINGS_H
