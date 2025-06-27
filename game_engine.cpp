/**************************************************************************************************
 * NationBuilder Game Engine
 *
 * Version: 2.0
 * Last Updated: 2025-06-27
 *
 * This engine serves as the core for a real-time nation-building simulation game. It features a
 * modular design that handles key gameplay systems independently.
 *
 * Core Features:
 * - Unit Simulation: Manages unit creation, movement via A* pathfinding, and state.
 * - Combat Simulation: A placeholder for deterministic or probabilistic combat logic.
 * - Economic Model: Tracks and updates the national economy.
 * - Government & Policy: Simulates political changes and their effects.
 * - Thread-Safe Chat: A simple, non-blocking console chat system for player interaction.
 *
 * This file is designed to be self-contained and provides a complete, runnable engine core.
 * Areas marked with `// TODO:` are intended for expansion with game-specific logic or
 * network integration.
 **************************************************************************************************/

// Standard Library Includes
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <functional>
#include <memory>
#include <string>

// C-style headers for specific functions
#include <cfloat> // For FLT_MAX

// Use a dedicated namespace to avoid polluting the global namespace.
namespace GameEngine {

/*************** Stage 1: Core Utilities & Base Classes ****************/

/**
 * @brief Gets a formatted timestamp string [HH:MM:SS].
 * @return A string representing the current time.
 */
std::string getTimestamp() {
    time_t now = time(nullptr);
    struct tm ltm;
    // Use platform-specific safe functions for localtime
#ifdef _WIN32
    localtime_s(&ltm, &now);
#else
    localtime_r(&now, &ltm);
#endif
    char buf[16];
    snprintf(buf, sizeof(buf), "[%02d:%02d:%02d]", ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
    return std::string(buf);
}

/**
 * @brief Logs a message to the console with a timestamp.
 * @param msg The message to log.
 */
void logEvent(const std::string &msg) {
    // This is thread-safe because cout is synchronized by default.
    std::cout << getTimestamp() << " " << msg << std::endl;
    // TODO: Replace with a more robust logging system (e.g., file or network logger).
}

/**
 * @class Module
 * @brief Abstract base class for all engine subsystems.
 *
 * Defines a common interface for initialization, updating, and shutdown,
 * allowing the main engine to manage all modules polymorphically.
 */
class Module {
public:
    virtual ~Module() = default; // Use default virtual destructor
    virtual bool init() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
};

/*************** Stage 2: Unit Module (with A* Pathfinding) ****************/

class UnitModule : public Module {
public:
    // Represents a single controllable unit in the game world.
    struct Unit {
        std::string name;
        int health;
        int x, y; // Current coordinates
        int destX, destY; // Destination coordinates
        std::vector<std::pair<int, int>> path;
        bool isMoving;

        Unit(const std::string &n, int h, int startX, int startY)
            : name(n), health(h), x(startX), y(startY), destX(startX), destY(startY), isMoving(false) {}
    };

private:
    std::vector<Unit> units;
    std::vector<std::vector<int>> grid; // Game world grid: 0 = traversable, 1 = obstacle
    int gridWidth, gridHeight;
    std::mutex unitMutex; // Protects access to the units vector.

    // A* Pathfinding Implementation
    struct Node {
        int x, y;
        float g, h, f; // A* costs: g=cost from start, h=heuristic to goal, f=g+h
        int parentX, parentY;

        Node(int _x = 0, int _y = 0, float _g = FLT_MAX, float _h = FLT_MAX, int pX = -1, int pY = -1)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parentX(pX), parentY(pY) {}

        // Comparison for priority queue (we want the node with the lowest f cost)
        bool operator>(const Node& other) const {
            return f > other.f;
        }
    };

    /**
     * @brief Computes the optimal path from a start to a goal using A*.
     * @param startX Starting X coordinate.
     * @param startY Starting Y coordinate.
     * @param goalX Destination X coordinate.
     * @param goalY Destination Y coordinate.
     * @return A vector of (x, y) pairs representing the path. Empty if no path is found.
     */
    std::vector<std::pair<int, int>> computePath(int startX, int startY, int goalX, int goalY) {
        auto heuristic = [&](int x, int y) {
            // Manhattan distance heuristic
            return static_cast<float>(std::abs(x - goalX) + std::abs(y - goalY));
        };

        std::vector<std::vector<bool>> closedSet(gridHeight, std::vector<bool>(gridWidth, false));
        std::vector<std::vector<Node>> allNodes(gridHeight, std::vector<Node>(gridWidth));
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;

        allNodes[startY][startX] = Node(startX, startY, 0, heuristic(startX, startY), -1, -1);
        openSet.push(allNodes[startY][startX]);

        bool pathFound = false;

        while (!openSet.empty()) {
            Node current = openSet.top();
            openSet.pop();

            if (current.x == goalX && current.y == goalY) {
                pathFound = true;
                break;
            }

            if (closedSet[current.y][current.x]) continue;
            closedSet[current.y][current.x] = true;

            // Explore neighbors (4-directional movement)
            int dx[] = {0, 0, -1, 1};
            int dy[] = {-1, 1, 0, 0};

            for (int i = 0; i < 4; ++i) {
                int nx = current.x + dx[i];
                int ny = current.y + dy[i];

                // Check bounds and obstacles
                if (nx < 0 || nx >= gridWidth || ny < 0 || ny >= gridHeight || grid[ny][nx] == 1 || closedSet[ny][nx]) {
                    continue;
                }

                float gNew = current.g + 1.0f;
                if (gNew < allNodes[ny][nx].g) {
                    allNodes[ny][nx] = Node(nx, ny, gNew, heuristic(nx, ny), current.x, current.y);
                    openSet.push(allNodes[ny][nx]);
                }
            }
        }

        // Reconstruct path from goal to start
        std::vector<std::pair<int, int>> path;
        if (pathFound) {
            int cx = goalX;
            int cy = goalY;
            while (cx != startX || cy != startY) {
                path.push_back({cx, cy});
                const Node& node = allNodes[cy][cx];
                cx = node.parentX;
                cy = node.parentY;
            }
            std::reverse(path.begin(), path.end());
        }
        return path;
    }

public:
    bool init() override {
        gridWidth = 20;
        gridHeight = 20;
        grid.assign(gridHeight, std::vector<int>(gridWidth, 0));

        // Create a simple obstacle wall
        for (int i = 5; i < 15; ++i) {
            grid[10][i] = 1;
        }

        // Initialize some units for demonstration
        units.emplace_back("Infantry", 100, 1, 1);
        units.emplace_back("Tank", 150, 2, 2);
        units.emplace_back("Artillery", 80, 3, 1);
        logEvent("UnitModule: Initialized with 3 units.");
        return true;
    }

    void update() override {
        std::lock_guard<std::mutex> lock(unitMutex);
        for (auto &unit : units) {
            if (unit.isMoving && !unit.path.empty()) {
                auto nextStep = unit.path.front();
                unit.path.erase(unit.path.begin());
                unit.x = nextStep.first;
                unit.y = nextStep.second;

                logEvent("Unit " + unit.name + " moved to (" + std::to_string(unit.x) + "," + std::to_string(unit.y) + ")");

                if (unit.path.empty()) {
                    unit.isMoving = false;
                    logEvent("Unit " + unit.name + " has reached its destination.");
                }
            }
        }
    }

    void shutdown() override {
        std::lock_guard<std::mutex> lock(unitMutex);
        units.clear();
        logEvent("UnitModule: Shutdown complete.");
    }

    // Public interface to command units
    void setDestination(size_t unitIndex, int destX, int destY) {
        std::lock_guard<std::mutex> lock(unitMutex);
        if (unitIndex >= units.size()) return;

        Unit &unit = units[unitIndex];
        unit.destX = destX;
        unit.destY = destY;
        unit.path = computePath(unit.x, unit.y, destX, destY);
        unit.isMoving = !unit.path.empty();

        if (unit.isMoving) {
            logEvent("Unit " + unit.name + " starting path to (" + std::to_string(destX) + "," + std::to_string(destY) + ")");
        } else {
            logEvent("Unit " + unit.name + " could not find a path to destination.");
        }
    }

    void printStatus() const {
        // Use a const_cast or a mutable mutex if you need to lock in a const function.
        // Or, better, make the calling context responsible for locking if needed.
        // For this simple case, we'll assume the caller handles thread safety.
        std::cout << "\n----- Unit Module Status -----" << std::endl;
        for (const auto &unit : units) {
            std::cout << "  - " << unit.name
                      << "\tHP: " << unit.health
                      << "\tPos: (" << unit.x << "," << unit.y << ")"
                      << "\tDest: (" << unit.destX << "," << unit.destY << ")"
                      << (unit.isMoving ? " [Moving]" : " [Idle]") << std::endl;
        }
        std::cout << "------------------------------\n" << std::endl;
    }
};

/*************** Stage 3: Combat Module ****************/

class CombatModule : public Module {
    std::mutex combatMutex;
public:
    bool init() override {
        logEvent("CombatModule: Initialized.");
        // TODO: Load weapon stats, armor types, and damage formulas from data files (e.g., JSON, XML).
        return true;
    }

    void update() override {
        std::lock_guard<std::mutex> lock(combatMutex);
        // TODO: Implement combat logic.
        //  1. Proximity Check: Identify units from opposing factions within attack range.
        //  2. Target Selection: Apply AI logic to choose targets (e.g., lowest health, biggest threat).
        //  3. Damage Calculation: Use formulas (e.g., Attack - Defense) and random factors.
        //  4. Apply Damage: Update unit health and handle unit death.
        if (rand() % 200 < 5) { // Low probability placeholder event
            logEvent("Combat: A skirmish was simulated.");
        }
    }

    void shutdown() override {
        logEvent("CombatModule: Shutdown complete.");
        // TODO: Clean up any allocated combat resources.
    }
};

/*************** Stage 4: Economy Module ****************/

class EconomyModule : public Module {
    double nationalTreasury;
    std::mutex econMutex;
public:
    bool init() override {
        nationalTreasury = 10000.0;
        logEvent("EconomyModule: Initialized with treasury of " + std::to_string(nationalTreasury));
        return true;
    }

    void update() override {
        std::lock_guard<std::mutex> lock(econMutex);
        // TODO: Implement a more sophisticated economic model.
        //  - Income: Taxes from population, trade tariffs, resource sales.
        //  - Expenses: Unit upkeep, building maintenance, research costs.
        //  - Growth: Factors like infrastructure, policies, and events should affect GDP.
        double growth = (rand() % 20 - 5) * 0.5; // Simulate minor fluctuations
        nationalTreasury += growth;

        if (rand() % 150 < 10) { // Occasional log
            logEvent("Economy: Treasury updated to " + std::to_string(nationalTreasury));
        }
    }

    void shutdown() override {
        // Persist final economic state to a file.
        std::ofstream ofs("economy_shutdown_state.txt");
        if (ofs) {
            ofs << "Final National Treasury: " << nationalTreasury << std::endl;
        }
        logEvent("EconomyModule: Shutdown complete. State saved.");
    }
};

/*************** Stage 5: Government Module ****************/

class GovernmentModule : public Module {
    std::string currentPolicy;
    int ticksSinceLastChange;
    std::mutex govMutex;
public:
    bool init() override {
        currentPolicy = "Neutral";
        ticksSinceLastChange = 0;
        logEvent("GovernmentModule: Initialized with policy: " + currentPolicy);
        return true;
    }

    void update() override {
        std::lock_guard<std::mutex> lock(govMutex);
        ticksSinceLastChange++;
        // TODO: Base policy changes on game events, player choices, or internal pressures (e.g., low stability).
        if (ticksSinceLastChange > 200) {
            currentPolicy = (currentPolicy == "Neutral") ? "Expansionist" : "Neutral";
            logEvent("Government: Policy has shifted to '" + currentPolicy + "'.");
            ticksSinceLastChange = 0;
        }
    }

    void shutdown() override {
        logEvent("GovernmentModule: Shutdown complete.");
        // TODO: Save final government state if necessary.
    }
};

/*************** Stage 6: Chat Module (Console Text Chat) ****************/

class ChatModule : public Module {
    std::vector<std::string> messageQueue;
    std::mutex chatMutex;
    std::thread inputThread;
    std::atomic<bool> isRunning;
public:
    bool init() override {
        isRunning.store(true);
        // Start a detached thread to handle blocking console input without pausing the engine.
        inputThread = std::thread(&ChatModule::inputLoop, this);
        logEvent("ChatModule: Initialized. Type '/exit' in the console to stop chat input.");
        return true;
    }

    void inputLoop() {
        std::string line;
        while (isRunning.load()) {
            std::cout << "> ";
            if (std::getline(std::cin, line)) {
                if (!line.empty()) {
                    if (line == "/exit") {
                        logEvent("Chat input thread exiting.");
                        break; // Exit the loop but don't stop the whole module yet
                    }
                    // TODO: In a networked game, send this message to a server.
                    addMessage("Player: " + line);
                }
            } else {
                 // Check if the main module is shutting down
                if (!isRunning.load()) break;
                // Sleep briefly to prevent busy-waiting if cin fails
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    void addMessage(const std::string &msg) {
        std::lock_guard<std::mutex> lock(chatMutex);
        messageQueue.push_back(msg);
    }

    void update() override {
        std::lock_guard<std::mutex> lock(chatMutex);
        if (!messageQueue.empty()) {
            std::cout << "\n------ Chat Log ------" << std::endl;
            for (const auto &msg : messageQueue) {
                std::cout << getTimestamp() << " " << msg << std::endl;
            }
             std::cout << "----------------------" << std::endl;
            messageQueue.clear();
        }
    }

    void shutdown() override {
        isRunning.store(false);
        // A clean way to unblock getline is needed, but for console it's tricky.
        // Prompting the user to press enter is a simple workaround.
        std::cout << "Press ENTER to fully shut down chat module." << std::endl;
        if (inputThread.joinable()) {
            inputThread.join();
        }
        logEvent("ChatModule: Shutdown complete.");
    }
};


/*************** Stage 7: GameEngine Orchestrator ****************/

class GameEngineController {
private:
    std::vector<std::unique_ptr<Module>> modules;
    std::atomic<bool> isEngineRunning;
    std::thread engineThread;

public:
    GameEngineController() : isEngineRunning(false) {}
    ~GameEngineController() {
        shutdown(); // Ensure cleanup on destruction
    }

    bool init() {
        // Use smart pointers for automatic memory management.
        modules.push_back(std::make_unique<UnitModule>());
        modules.push_back(std::make_unique<CombatModule>());
        modules.push_back(std::make_unique<EconomyModule>());
        modules.push_back(std::make_unique<GovernmentModule>());
        modules.push_back(std::make_unique<ChatModule>());

        for (const auto& mod : modules) {
            if (!mod->init()) {
                logEvent("GameEngineController: Failed to initialize a module.");
                return false;
            }
        }
        isEngineRunning.store(true);
        logEvent("GameEngineController: All modules initialized successfully.");
        return true;
    }

    void run() {
        if (!isEngineRunning) return;
        engineThread = std::thread(&GameEngineController::mainLoop, this);
        logEvent("GameEngineController: Main loop started.");
    }

    void mainLoop() {
        const std::chrono::milliseconds tickRate(33); // ~30 FPS
        long long iteration = 0;

        while (isEngineRunning.load()) {
            auto startTime = std::chrono::steady_clock::now();

            // Update all modules
            for (const auto& mod : modules) {
                mod->update();
            }

            // Periodic status updates
            if (iteration % 150 == 0) {
                // Safely get the UnitModule to print status
                if (auto um = dynamic_cast<UnitModule*>(modules[0].get())) {
                    um->printStatus();
                }
            }

            auto endTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

            // Maintain a consistent tick rate
            if (elapsedTime < tickRate) {
                std::this_thread::sleep_for(tickRate - elapsedTime);
            }
            
            iteration++;

            // For demonstration, automatically stop after a certain number of iterations.
            // In a real game, this would be controlled by user input (e.g., quitting the game).
            if (iteration > 1500) {
                logEvent("GameEngineController: Demo loop finished. Initiating shutdown.");
                isEngineRunning.store(false);
            }
        }
    }

    void stop() {
        isEngineRunning.store(false);
    }
    
    // Allows external systems to access modules if necessary.
    template<typename T>
    T* getModule() {
        for (const auto& mod : modules) {
            if (T* specificModule = dynamic_cast<T*>(mod.get())) {
                return specificModule;
            }
        }
        return nullptr;
    }


    void shutdown() {
        if (engineThread.joinable()) {
            engineThread.join();
        }

        // Shutdown modules in reverse order of initialization
        for (auto it = modules.rbegin(); it != modules.rend(); ++it) {
            (*it)->shutdown();
        }
        modules.clear(); // Smart pointers handle deletion
        logEvent("GameEngineController: Engine shutdown complete.");
    }
};

} // namespace GameEngine

/*************** Stage 8: Main Application Entry Point ****************/

int main() {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    GameEngine::logEvent("NationBuilder Game Engine starting...");

    // Create the main engine controller
    auto engine = std::make_unique<GameEngine::GameEngineController>();

    if (!engine->init()) {
        GameEngine::logEvent("Engine initialization failed. Exiting.");
        return 1;
    }

    // Example of interacting with a module post-initialization
    if (auto unitModule = engine->getModule<GameEngine::UnitModule>()) {
        unitModule->setDestination(0, 18, 18); // Send Infantry to a corner
        unitModule->setDestination(1, 8, 9);  // Send Tank towards the wall
    }

    // Start the main game loop and wait for it to finish
    engine->run();
    
    // The engine's destructor will handle the shutdown call automatically when main exits.
    // If you need more control, you can explicitly call engine->shutdown().
    
    GameEngine::logEvent("NationBuilder Game Engine terminated.");
    return 0;
}
