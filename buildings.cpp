/*
 * buildings.cpp - Refactored Production-Quality Buildings Module for Conqueror Engine
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>

// Logging utility
inline void logEvent(const std::string &message, const std::string &level = "INFO") {
    std::cout << "[" << level << "] " << message << std::endl;
}

struct BuildingVariant {
    std::string category;
    std::string variantName;
    double cost;
    double upgradeCost;
    double buildTime;
    double productionBonus;
    bool subscriptionRequired;
    std::string iconPath;
};

std::map<std::string, std::vector<BuildingVariant>> g_buildingVariants;

void initBuildingVariants(); // forward declaration

class Building {
public:
    std::string category;
    BuildingVariant variant;
    int level = 1;
    double health = 1000.0;

    Building(const std::string &cat, const BuildingVariant &var)
        : category(cat), variant(var) {
        logEvent("Created: " + var.variantName, "DEBUG");
    }

    virtual ~Building() = default;

    virtual void upgrade() {
        ++level;
        health *= 1.2;
        logEvent("Upgraded to level " + std::to_string(level), "INFO");
    }

    virtual double produce() { return 0.0; }

    virtual std::string getInfo() const {
        std::ostringstream oss;
        oss << category << " - " << variant.variantName << " [Lvl: " << level << ", HP: "
            << std::fixed << std::setprecision(1) << health << "]";
        return oss.str();
    }
};

class ResourceMine : public Building {
public:
    explicit ResourceMine(const BuildingVariant &var) : Building("Resource Mine", var) {}

    double produce() override {
        double base = 100.0 * variant.productionBonus * level;
        logEvent(variant.variantName + " produced " + std::to_string(base), "DEBUG");
        return base;
    }

    std::string getInfo() const override {
        return Building::getInfo() + ", Bonus: " + std::to_string(variant.productionBonus);
    }
};

class BuildingManager {
    std::vector<std::unique_ptr<Building>> buildings;
    std::mutex mtx;

public:
    Building* buyBuilding(const std::string &category, double &nationTreasury) {
        auto it = g_buildingVariants.find(category);
        if (it == g_buildingVariants.end() || it->second.empty()) {
            logEvent("Category not found: " + category, "ERROR");
            return nullptr;
        }

        const auto &variant = it->second.front();
        if (nationTreasury < variant.cost) {
            logEvent("Insufficient funds for " + category, "WARN");
            return nullptr;
        }

        nationTreasury -= variant.cost;
        std::unique_ptr<Building> b;
        b = (category == "Resource Mine") ? std::make_unique<ResourceMine>(variant)
                                           : std::make_unique<Building>(category, variant);
        Building* raw = b.get();

        {
            std::lock_guard<std::mutex> lock(mtx);
            buildings.push_back(std::move(b));
        }
        logEvent("Purchased: " + category);
        return raw;
    }

    bool upgradeBuilding(int index, double &nationTreasury) {
        std::lock_guard<std::mutex> lock(mtx);
        if (index < 0 || index >= static_cast<int>(buildings.size())) {
            logEvent("Invalid index for upgrade", "ERROR");
            return false;
        }

        auto &b = buildings[index];
        double cost = b->variant.upgradeCost * b->level;
        if (nationTreasury < cost) {
            logEvent("Upgrade too costly: $" + std::to_string(cost), "WARN");
            return false;
        }

        nationTreasury -= cost;
        b->upgrade();
        return true;
    }

    double simulateProduction() {
        double total = 0.0;
        std::lock_guard<std::mutex> lock(mtx);
        for (auto &b : buildings) total += b->produce();
        return total;
    }

    void dumpBuildings() {
        std::lock_guard<std::mutex> lock(mtx);
        for (size_t i = 0; i < buildings.size(); ++i)
            logEvent("[" + std::to_string(i) + "] " + buildings[i]->getInfo(), "DEBUG");
    }
};

void initBuildingVariants() {
    g_buildingVariants["Barracks"] = {
        {"Barracks", "M1 Infantry Barracks", 200000, 50000, 10, 1.0, false, "icons/barracks_m1.png"},
        {"Barracks", "Next-Gen Elite Command Center", 500000, 100000, 20, 1.5, true, "icons/barracks_elite.png"}
    };
    g_buildingVariants["Factory"] = {
        {"Factory", "Conventional Arms Factory", 400000, 120000, 20, 1.0, false, "icons/factory_conventional.png"}
    };
    g_buildingVariants["Resource Mine"] = {
        {"Resource Mine", "Standard Gold Mine", 180000, 50000, 7, 1.0, false, "icons/mine_gold.png"}
    };
    // Add others as needed...
}

#ifdef BUILDINGS_TEST
int main() {
    initBuildingVariants();
    double treasury = 1'000'000;
    BuildingManager manager;
    manager.buyBuilding("Barracks", treasury);
    manager.buyBuilding("Resource Mine", treasury);
    manager.upgradeBuilding(1, treasury);
    manager.dumpBuildings();
    std::cout << "Total production: " << manager.simulateProduction() << std::endl;
}
#endif
