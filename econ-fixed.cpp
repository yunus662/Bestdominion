/********************************************************************************************************************
 * econ_fixed.cpp
 * Production‑Quality Economy Manager Module for Conqueror Engine (C++ Version)
 * 
 * This module implements an advanced economy management system that tracks and updates the in‑game resources,
 * including gold, food, wood, iron, uranium, oil, fuel, and diamonds. It simulates resource production,
 * adjustments due to economic events, and spending of resources by various game systems.
 * 
 * Key Features:
 *   - EconomyManager class to encapsulate all resource operations.
 *   - Functions to produce resources periodically, update resource stocks based on in‑game events,
 *     and apply cost modifiers from technologies or doctrines.
 *   - Detailed logging for monitoring resource updates and debugging.
 *   - A test harness (guarded by ECON_TEST) to simulate various economic scenarios.
 * 
 * Build with (for example):
 *   g++ econ_fixed.cpp -o econ_fixed -std=c++11 -pthread
 ********************************************************************************************************************/

#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>

// -------------------------------------------------
// Logging utility: Simple logEvent function.
void logEvent(const std::string &message, const std::string &level = "INFO") {
    std::cout << "[" << level << "] " << message << std::endl;
}

// -------------------------------------------------
// EconomyManager: Manages in-game resources.
class EconomyManager {
public:
    // Resource inventory.
    double gold;
    double food;
    double wood;
    double iron;
    double uranium;
    double oil;
    double fuel;
    double diamonds;
    
    // Production rates per game minute.
    double goldRate;
    double foodRate;
    double woodRate;
    double ironRate;
    double uraniumRate;
    double oilRate;
    double fuelRate;
    double diamondsRate;
    
    // Mutex for safe multithreaded operations.
    std::mutex mtx;
    
    EconomyManager();
    ~EconomyManager();
    
    // Produce resources based on production rates.
    void produceResources(double gameMinutes);
    
    // Spend resources. Returns true if spending succeeded.
    bool spendResource(const std::string &resource, double amount);
    
    // Adjust production rates; can be called to simulate events or upgrades.
    void adjustProduction(const std::string &resource, double deltaRate);
    
    // Display current resources.
    std::string getResourceReport();
    
    // Reset resources to initial values.
    void reset();
};

EconomyManager::EconomyManager()
    : gold(1000000), food(500000), wood(300000), iron(200000),
      uranium(100000), oil(400000), fuel(250000), diamonds(50000),
      goldRate(10000), foodRate(5000), woodRate(3000), ironRate(2000),
      uraniumRate(1000), oilRate(4000), fuelRate(2500), diamondsRate(500)
{
    logEvent("EconomyManager initialized with starting resources.", "DEBUG");
}

EconomyManager::~EconomyManager() {
    logEvent("EconomyManager terminated.", "DEBUG");
}

void EconomyManager::produceResources(double gameMinutes) {
    std::lock_guard<std::mutex> lock(mtx);
    gold += goldRate * gameMinutes;
    food += foodRate * gameMinutes;
    wood += woodRate * gameMinutes;
    iron += ironRate * gameMinutes;
    uranium += uraniumRate * gameMinutes;
    oil += oilRate * gameMinutes;
    fuel += fuelRate * gameMinutes;
    diamonds += diamondsRate * gameMinutes;
    
    std::ostringstream oss;
    oss << "Produced resources over " << gameMinutes << " game minutes.";
    logEvent(oss.str(), "DEBUG");
}

bool EconomyManager::spendResource(const std::string &resource, double amount) {
    std::lock_guard<std::mutex> lock(mtx);
    double *resPtr = nullptr;
    if (resource == "gold") resPtr = &gold;
    else if (resource == "food") resPtr = &food;
    else if (resource == "wood") resPtr = &wood;
    else if (resource == "iron") resPtr = &iron;
    else if (resource == "uranium") resPtr = &uranium;
    else if (resource == "oil") resPtr = &oil;
    else if (resource == "fuel") resPtr = &fuel;
    else if (resource == "diamonds") resPtr = &diamonds;
    
    if (!resPtr) {
        logEvent("Attempted to spend unknown resource: " + resource, "ERROR");
        return false;
    }
    
    if (*resPtr >= amount) {
        *resPtr -= amount;
        std::ostringstream oss;
        oss << "Spent " << amount << " of " << resource << ". New balance: " << *resPtr;
        logEvent(oss.str(), "DEBUG");
        return true;
    } else {
        std::ostringstream oss;
        oss << "Insufficient " << resource << ": Required " << amount << ", Available " << *resPtr;
        logEvent(oss.str(), "WARNING");
        return false;
    }
}

void EconomyManager::adjustProduction(const std::string &resource, double deltaRate) {
    std::lock_guard<std::mutex> lock(mtx);
    if (resource == "gold") goldRate += deltaRate;
    else if (resource == "food") foodRate += deltaRate;
    else if (resource == "wood") woodRate += deltaRate;
    else if (resource == "iron") ironRate += deltaRate;
    else if (resource == "uranium") uraniumRate += deltaRate;
    else if (resource == "oil") oilRate += deltaRate;
    else if (resource == "fuel") fuelRate += deltaRate;
    else if (resource == "diamonds") diamondsRate += deltaRate;
    else {
        logEvent("Unknown resource in adjustProduction: " + resource, "ERROR");
        return;
    }
    
    std::ostringstream oss;
    oss << "Production rate for " << resource << " adjusted by " << deltaRate;
    logEvent(oss.str(), "DEBUG");
}

std::string EconomyManager::getResourceReport() {
    std::lock_guard<std::mutex> lock(mtx);
    std::ostringstream oss;
    oss << "Resources Report:\n";
    oss << " Gold: " << std::fixed << std::setprecision(2) << gold << "\n";
    oss << " Food: " << food << "\n";
    oss << " Wood: " << wood << "\n";
    oss << " Iron: " << iron << "\n";
    oss << " Uranium: " << uranium << "\n";
    oss << " Oil: " << oil << "\n";
    oss << " Fuel: " << fuel << "\n";
    oss << " Diamonds: " << diamonds << "\n";
    return oss.str();
}

void EconomyManager::reset() {
    std::lock_guard<std::mutex> lock(mtx);
    gold = 1000000;
    food = 500000;
    wood = 300000;
    iron = 200000;
    uranium = 100000;
    oil = 400000;
    fuel = 250000;
    diamonds = 50000;
    logEvent("Resources reset to initial values.", "DEBUG");
}

// -------------------------------------------------
// Standalone Testing Block for EconomyManager
#ifdef ECON_TEST
#include <thread>
int main() {
    EconomyManager econ;
    logEvent(econ.getResourceReport());
    
    // Simulate resource production over 10 game minutes.
    econ.produceResources(10);
    logEvent(econ.getResourceReport());
    
    // Attempt to spend some resources.
    econ.spendResource("gold", 300000);
    econ.spendResource("food", 100000);
    logEvent(econ.getResourceReport());
    
    // Adjust production rates.
    econ.adjustProduction("gold", 5000);
    econ.adjustProduction("food", 2000);
    
    // Produce again.
    econ.produceResources(5);
    logEvent(econ.getResourceReport());
    
    // Reset economy.
    econ.reset();
    logEvent(econ.getResourceReport());
    
    return 0;
}
#endif

// End of econ_fixed.cpp
