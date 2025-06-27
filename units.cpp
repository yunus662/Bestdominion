// units.cpp
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iomanip>

// -------------------------------------------------
// Logging utility (simulating logEvent from JS)
void logEvent(const std::string &message) {
    std::cout << message << std::endl;
}

// -------------------------------------------------
// Structure representing a unit variant
struct UnitVariant {
    std::string category;      // For example: "Tank", "Infantry", etc.
    std::string variantName;   // Real-life variant name.
    double cost;               // Money cost.
    double resourceCost;       // Additional resource cost.
    bool subscriptionRequired; // True if requires tickets/subscription.
    std::string iconPath;      // Icon file path.
};

// Global registry mapping unit category to its variants.
std::map<std::string, std::vector<UnitVariant>> g_unitVariants;

// -------------------------------------------------
// Function to initialize unit variants for all categories.
void initUnitVariants() {
    // --- Tanks ---
    std::vector<UnitVariant> tanks;
    tanks.push_back({"Tank", "M1 Abrams", 1000000, 500000, false, "icons/tank_m1.png"});
    tanks.push_back({"Tank", "Leopard 2A7", 1100000, 550000, false, "icons/tank_leopard2a7.png"});
    tanks.push_back({"Tank", "T-14 Armata", 1200000, 600000, false, "icons/tank_t14.png"});
    tanks.push_back({"Tank", "Challenger 3", 1300000, 650000, false, "icons/tank_challenger3.png"});
    tanks.push_back({"Tank", "Merkava Mk.4", 1400000, 700000, false, "icons/tank_merkava.png"});
    tanks.push_back({"Tank", "K2 Black Panther", 1500000, 750000, false, "icons/tank_k2.png"});
    tanks.push_back({"Tank", "Type 10", 2000000, 1000000, true, "icons/tank_type10.png"});
    g_unitVariants["Tank"] = tanks;

    // --- Infantry ---
    std::vector<UnitVariant> infantry;
    infantry.push_back({"Infantry", "Standard Infantry Soldier", 50000, 25000, false, "icons/infantry_standard.png"});
    infantry.push_back({"Infantry", "Mechanized Infantry", 75000, 37500, false, "icons/infantry_mechanized.png"});
    infantry.push_back({"Infantry", "Airborne Infantry", 100000, 50000, false, "icons/infantry_airborne.png"});
    infantry.push_back({"Infantry", "Special Forces Operative", 125000, 60000, false, "icons/infantry_special.png"});
    infantry.push_back({"Infantry", "Urban Warfare Infantry", 150000, 75000, false, "icons/infantry_urban.png"});
    infantry.push_back({"Infantry", "High-Tech Robot Infantry", 175000, 87500, false, "icons/infantry_robot.png"});
    infantry.push_back({"Infantry", "Exoskeleton-Assisted Elite Infantry", 250000, 125000, true, "icons/infantry_exo.png"});
    g_unitVariants["Infantry"] = infantry;

    // --- Fighter Jets ---
    std::vector<UnitVariant> fighterJets;
    fighterJets.push_back({"Fighter Jet", "F-16 Fighting Falcon", 800000, 400000, false, "icons/fighter_f16.png"});
    fighterJets.push_back({"Fighter Jet", "F/A-18 Hornet", 850000, 425000, false, "icons/fighter_f18.png"});
    fighterJets.push_back({"Fighter Jet", "MiG-29 Fulcrum", 900000, 450000, false, "icons/fighter_mig29.png"});
    fighterJets.push_back({"Fighter Jet", "Dassault Mirage 2000", 950000, 475000, false, "icons/fighter_mirage2000.png"});
    fighterJets.push_back({"Fighter Jet", "Sukhoi Su-27 Flanker", 1000000, 500000, false, "icons/fighter_su27.png"});
    fighterJets.push_back({"Fighter Jet", "Eurofighter Typhoon", 1100000, 550000, false, "icons/fighter_typhoon.png"});
    fighterJets.push_back({"Fighter Jet", "F-15E Strike Eagle", 1500000, 750000, true, "icons/fighter_f15e.png"});
    g_unitVariants["Fighter Jet"] = fighterJets;

    // --- Stealth Fighter Jets ---
    std::vector<UnitVariant> stealthFighterJets;
    stealthFighterJets.push_back({"Stealth Fighter Jet", "F-22 Raptor", 2000000, 1000000, false, "icons/stealth_f22.png"});
    stealthFighterJets.push_back({"Stealth Fighter Jet", "F-35 Lightning II", 2100000, 1050000, false, "icons/stealth_f35.png"});
    stealthFighterJets.push_back({"Stealth Fighter Jet", "Chengdu J-20", 2200000, 1100000, false, "icons/stealth_j20.png"});
    stealthFighterJets.push_back({"Stealth Fighter Jet", "Sukhoi Su-57", 2300000, 1150000, false, "icons/stealth_su57.png"});
    stealthFighterJets.push_back({"Stealth Fighter Jet", "Mikoyan MiG-41", 2400000, 1200000, false, "icons/stealth_mig41.png"});
    stealthFighterJets.push_back({"Stealth Fighter Jet", "Dassault nEUROn", 2500000, 1250000, false, "icons/stealth_neuron.png"});
    stealthFighterJets.push_back({"Stealth Fighter Jet", "Future X-Stealth Fighter", 3500000, 1750000, true, "icons/stealth_future.png"});
    g_unitVariants["Stealth Fighter Jet"] = stealthFighterJets;

    // --- Helicopters ---
    std::vector<UnitVariant> helicopters;
    helicopters.push_back({"Helicopter", "AH-64 Apache", 600000, 300000, false, "icons/helicopter_apache.png"});
    helicopters.push_back({"Helicopter", "Bell UH-1Y Venom", 650000, 325000, false, "icons/helicopter_bell.png"});
    helicopters.push_back({"Helicopter", "Eurocopter Tiger", 700000, 350000, false, "icons/helicopter_tiger.png"});
    helicopters.push_back({"Helicopter", "Kamov Ka-52", 750000, 375000, false, "icons/helicopter_ka52.png"});
    helicopters.push_back({"Helicopter", "Sikorsky CH-53K King Stallion", 800000, 400000, false, "icons/helicopter_ch53k.png"});
    helicopters.push_back({"Helicopter", "Boeing MH-47E Chinook", 850000, 425000, false, "icons/helicopter_chinook.png"});
    helicopters.push_back({"Helicopter", "Future Stealth Attack Helicopter", 1200000, 600000, true, "icons/helicopter_future.png"});
    g_unitVariants["Helicopter"] = helicopters;

    // --- Warships ---
    std::vector<UnitVariant> warships;
    warships.push_back({"Warship", "Arleigh Burke-class Destroyer", 2000000, 1000000, false, "icons/warship_abd.png"});
    warships.push_back({"Warship", "Zumwalt-class Destroyer", 2200000, 1100000, false, "icons/warship_zumwalt.png"});
    warships.push_back({"Warship", "Type 45 Destroyer", 2400000, 1200000, false, "icons/warship_type45.png"});
    warships.push_back({"Warship", "KDX-II Destroyer", 2600000, 1300000, false, "icons/warship_kdxii.png"});
    warships.push_back({"Warship", "Sejong the Great-class Destroyer", 2800000, 1400000, false, "icons/warship_sejong.png"});
    warships.push_back({"Warship", "INS Vishakhapatnam", 3000000, 1500000, false, "icons/warship_vishakhapatnam.png"});
    warships.push_back({"Warship", "Future Quantum-Class Warship", 4000000, 2000000, true, "icons/warship_future.png"});
    g_unitVariants["Warship"] = warships;

    // --- Artillery ---
    std::vector<UnitVariant> artillery;
    artillery.push_back({"Artillery", "M109 Paladin", 300000, 150000, false, "icons/artillery_paladin.png"});
    artillery.push_back({"Artillery", "PzH 2000", 320000, 160000, false, "icons/artillery_pzh2000.png"});
    artillery.push_back({"Artillery", "K9 Thunder", 340000, 170000, false, "icons/artillery_k9.png"});
    artillery.push_back({"Artillery", "G6 Howitzer", 360000, 180000, false, "icons/artillery_g6.png"});
    artillery.push_back({"Artillery", "M777 Howitzer", 380000, 190000, false, "icons/artillery_m777.png"});
    artillery.push_back({"Artillery", "D-30 Howitzer", 400000, 200000, false, "icons/artillery_d30.png"});
    artillery.push_back({"Artillery", "Next-Gen Automated Artillery System", 600000, 300000, true, "icons/artillery_future.png"});
    g_unitVariants["Artillery"] = artillery;

    // --- Radar ---
    std::vector<UnitVariant> radar;
    radar.push_back({"Radar", "AN/SPY-1", 500000, 250000, false, "icons/radar_spy1.png"});
    radar.push_back({"Radar", "Sea Giraffe 3D", 520000, 260000, false, "icons/radar_seagiraffe.png"});
    radar.push_back({"Radar", "AN/TPY-2", 540000, 270000, false, "icons/radar_tpy2.png"});
    radar.push_back({"Radar", "S1850M Long Range Radar", 560000, 280000, false, "icons/radar_s1850m.png"});
    radar.push_back({"Radar", "Ground Master 400", 580000, 290000, false, "icons/radar_groundmaster400.png"});
    radar.push_back({"Radar", "SMART-L EWC", 600000, 300000, false, "icons/radar_smartl.png"});
    radar.push_back({"Radar", "Quantum Radar X2000", 800000, 400000, true, "icons/radar_quantum.png"});
    g_unitVariants["Radar"] = radar;

    // --- Anti-Air Defense Systems ---
    std::vector<UnitVariant> antiAir;
    antiAir.push_back({"Anti-Air Defense", "Patriot Missile System", 1000000, 500000, false, "icons/antiair_patriot.png"});
    antiAir.push_back({"Anti-Air Defense", "S-400 Triumf", 1100000, 550000, false, "icons/antiair_s400.png"});
    antiAir.push_back({"Anti-Air Defense", "Aegis Combat System", 1200000, 600000, false, "icons/antiair_aegis.png"});
    antiAir.push_back({"Anti-Air Defense", "Iron Dome", 1300000, 650000, false, "icons/antiair_irondome.png"});
    antiAir.push_back({"Anti-Air Defense", "NASAMS", 1400000, 700000, false, "icons/antiair_nasams.png"});
    antiAir.push_back({"Anti-Air Defense", "Type 03 Chū-SAM", 1500000, 750000, false, "icons/antiair_type03.png"});
    antiAir.push_back({"Anti-Air Defense", "Next-Gen Laser Defense System", 2500000, 1250000, true, "icons/antiair_future.png"});
    g_unitVariants["Anti-Air Defense"] = antiAir;

    // --- Armored Vehicles ---
    std::vector<UnitVariant> armoredVehicles;
    armoredVehicles.push_back({"Armored Vehicle", "Stryker", 400000, 200000, false, "icons/armored_stryker.png"});
    armoredVehicles.push_back({"Armored Vehicle", "LAV-25", 420000, 210000, false, "icons/armored_lav25.png"});
    armoredVehicles.push_back({"Armored Vehicle", "BTR-80", 440000, 220000, false, "icons/armored_btr80.png"});
    armoredVehicles.push_back({"Armored Vehicle", "Patria AMV", 460000, 230000, false, "icons/armored_patriaamv.png"});
    armoredVehicles.push_back({"Armored Vehicle", "VBCI", 480000, 240000, false, "icons/armored_vbci.png"});
    armoredVehicles.push_back({"Armored Vehicle", "Piranha V", 500000, 250000, false, "icons/armored_piranhav.png"});
    armoredVehicles.push_back({"Armored Vehicle", "Future Autonomous Armored Vehicle", 700000, 350000, true, "icons/armored_future.png"});
    g_unitVariants["Armored Vehicle"] = armoredVehicles;

    // --- Missiles ---
    std::vector<UnitVariant> missiles;
    missiles.push_back({"Missile", "AGM-114 Hellfire", 300000, 150000, false, "icons/missile_hellfire.png"});
    missiles.push_back({"Missile", "AIM-120 AMRAAM", 320000, 160000, false, "icons/missile_amraam.png"});
    missiles.push_back({"Missile", "RBS-15", 340000, 170000, false, "icons/missile_rbs15.png"});
    missiles.push_back({"Missile", "Kh-31", 360000, 180000, false, "icons/missile_kh31.png"});
    missiles.push_back({"Missile", "Meteor", 380000, 190000, false, "icons/missile_meteor.png"});
    missiles.push_back({"Missile", "PL-15", 400000, 200000, false, "icons/missile_pl15.png"});
    missiles.push_back({"Missile", "Next-Gen Hypersonic Missile", 600000, 300000, true, "icons/missile_future.png"});
    g_unitVariants["Missile"] = missiles;

    // --- Missile Launchers ---
    std::vector<UnitVariant> missileLaunchers;
    missileLaunchers.push_back({"Missile Launcher", "MLRS", 350000, 175000, false, "icons/missile_launcher_mlrs.png"});
    missileLaunchers.push_back({"Missile Launcher", "Pinaka", 370000, 185000, false, "icons/missile_launcher_pinaka.png"});
    missileLaunchers.push_back({"Missile Launcher", "BM-30 Smerch", 390000, 195000, false, "icons/missile_launcher_smerch.png"});
    missileLaunchers.push_back({"Missile Launcher", "TOS-1 Buratino", 410000, 205000, false, "icons/missile_launcher_tos1.png"});
    missileLaunchers.push_back({"Missile Launcher", "HIMARS", 430000, 215000, false, "icons/missile_launcher_himars.png"});
    missileLaunchers.push_back({"Missile Launcher", "Smerch M", 450000, 225000, false, "icons/missile_launcher_smerchm.png"});
    missileLaunchers.push_back({"Missile Launcher", "Next-Gen Precision Launcher", 650000, 325000, true, "icons/missile_launcher_future.png"});
    g_unitVariants["Missile Launcher"] = missileLaunchers;
}

// -------------------------------------------------
// Define a simple Nation structure for simulation.
struct Nation {
    std::string name;
    double treasury;
    std::vector<std::unique_ptr<class Unit>> units;
};

// Global nations registry.
std::map<std::string, Nation> g_nations;

// Simulated getNationData function.
Nation* getNationData(const std::string &nationName) {
    auto it = g_nations.find(nationName);
    if (it != g_nations.end())
        return &it->second;
    else {
        Nation newNation;
        newNation.name = nationName;
        newNation.treasury = 10000000; // 10 million initial treasury.
        g_nations[nationName] = newNation;
        return &g_nations[nationName];
    }
}

// -------------------------------------------------
// Unit class representing a game unit.
class Unit {
public:
    std::string category;
    UnitVariant variant;
    float x, y; // 2D position.
    std::string nationName;
    
    Unit(const std::string &cat, const UnitVariant &var, float posX, float posY, const std::string &nation)
        : category(cat), variant(var), x(posX), y(posY), nationName(nation) {}
    
    void moveTo(float destX, float destY) {
        x = destX;
        y = destY;
        std::ostringstream oss;
        oss << "Unit (" << category << " - " << variant.variantName << ") moved to (" 
            << destX << ", " << destY << ")";
        logEvent(oss.str());
    }
    
    void printInfo() {
        std::cout << "Unit Info - Category: " << category 
                  << ", Variant: " << variant.variantName 
                  << ", Cost: $" << std::fixed << std::setprecision(2) << variant.cost 
                  << ", Subscription: " << (variant.subscriptionRequired ? "Yes" : "No")
                  << ", Position: (" << x << ", " << y << ")" << std::endl;
    }
};

// -------------------------------------------------
// buyUnit: Buys a unit of the specified category at (posX, posY) for a nation.
Unit* buyUnit(const std::string &unitCategory, float posX, float posY, const std::string &nationName) {
    Nation* nation = getNationData(nationName);
    if (!nation) {
        logEvent("Nation data not found for " + nationName);
        return nullptr;
    }
    
    if (g_unitVariants.find(unitCategory) == g_unitVariants.end()) {
        logEvent("Unit category " + unitCategory + " not found.");
        return nullptr;
    }
    
    auto &variants = g_unitVariants[unitCategory];
    if (variants.empty()) {
        logEvent("No variants available for " + unitCategory);
        return nullptr;
    }
    
    // Choose the cheapest variant (first variant).
    UnitVariant chosenVariant = variants.front();
    
    if (nation->treasury >= chosenVariant.cost) {
        nation->treasury -= chosenVariant.cost;
        Unit* newUnit = new Unit(unitCategory, chosenVariant, posX, posY, nationName);
        nation->units.push_back(std::unique_ptr<Unit>(newUnit));
        std::ostringstream oss;
        oss << nationName << " purchased " << unitCategory << " (" 
            << chosenVariant.variantName << ") for $" << std::fixed 
            << std::setprecision(2) << chosenVariant.cost;
        logEvent(oss.str());
        return newUnit;
    } else {
        std::ostringstream oss;
        oss << nationName << " cannot afford " << unitCategory 
            << " (" << chosenVariant.variantName << ") costing $" 
            << std::fixed << std::setprecision(2) << chosenVariant.cost;
        logEvent(oss.str());
        return nullptr;
    }
}

// -------------------------------------------------
// createUnit: Simulates unit marker creation (returns a Unit without deducting cost)
Unit* createUnit(const std::string &unitCategory, float posX, float posY, const std::string &iconPath, const std::string &nationName) {
    logEvent("Creating unit marker for " + unitCategory + " at (" + std::to_string(posX) + ", " + std::to_string(posY) + ")");
    if (g_unitVariants.find(unitCategory) == g_unitVariants.end() || g_unitVariants[unitCategory].empty()) {
        logEvent("Unit category " + unitCategory + " not available.");
        return nullptr;
    }
    UnitVariant variant = g_unitVariants[unitCategory].front();
    Unit* newUnit = new Unit(unitCategory, variant, posX, posY, nationName);
    return newUnit;
}

// -------------------------------------------------
// moveUnitTo: Moves the provided unit to (destX, destY)
void moveUnitTo(Unit* unit, float destX, float destY) {
    if (!unit) {
        logEvent("Invalid unit provided to moveUnitTo.");
        return;
    }
    unit->moveTo(destX, destY);
}

// -------------------------------------------------
// Main Testing Block (Compile with -DUNIT_TEST to run)
#ifdef UNIT_TEST
int main() {
    // Initialize unit variants.
    initUnitVariants();
    
    // Create a test nation.
    getNationData("TestLand");
    
    // Buy a Tank.
    Unit* myTank = buyUnit("Tank", 50.0f, 100.0f, "TestLand");
    if (myTank) {
        myTank->printInfo();
        moveUnitTo(myTank, 75.0f, 125.0f);
        myTank->printInfo();
    }
    
    // Buy a Fighter Jet.
    Unit* myFighter = buyUnit("Fighter Jet", 200.0f, 300.0f, "TestLand");
    if (myFighter) {
        myFighter->printInfo();
        moveUnitTo(myFighter, 250.0f, 350.0f);
        myFighter->printInfo();
    }
    
    return 0;
}
#endif

// End of units.cpp
