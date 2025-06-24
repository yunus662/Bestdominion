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
#include <limits>
#include <algorithm>
#include <functional>
using namespace std;

//------------------------------------------------------------------------------
// Utility Logging
//------------------------------------------------------------------------------
string getTimestamp() {
    time_t now = time(nullptr);
    struct tm *ltm = localtime(&now);
    char buf[16];
    sprintf(buf, "[%02d:%02d:%02d]", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return string(buf);
}

void logEvent(const string &msg) {
    cout << getTimestamp() << " " << msg << endl;
    // EXPAND: Replace with robust file/network logging if desired.
}

//------------------------------------------------------------------------------
// Module Abstract Base Class
//------------------------------------------------------------------------------
class Module {
public:
    virtual bool init() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
    virtual ~Module() {}
};

//------------------------------------------------------------------------------
// UnitModule - Manages Units and Pathfinding using A* Algorithm
//------------------------------------------------------------------------------
class UnitModule : public Module {
public:
    struct Unit {
        string name;
        int health;
        int x, y;
        int destX, destY;
        vector<pair<int,int>> path;
        bool moving;
        Unit(const string &n, int h, int sx, int sy)
            : name(n), health(h), x(sx), y(sy), destX(sx), destY(sy), moving(false) {}
    };
private:
    vector<Unit> units;
    vector<vector<int>> grid; // 0 = free, 1 = obstacle
    int gridWidth, gridHeight;
    mutex unitMutex;
public:
    bool init() override {
        // Initialize a grid map (20x20) with an obstacle row in the middle.
        gridWidth = 20; gridHeight = 20;
        grid.resize(gridHeight, vector<int>(gridWidth, 0));
        for (int i = 5; i < 15; i++)
            grid[10][i] = 1;
        // Initialize sample units.
        units.push_back(Unit("Infantry", 100, 1, 1));
        units.push_back(Unit("Tank", 150, 2, 2));
        return true;
    }
    
    // A* pathfinding algorithm: computes a path from (startX, startY) to (goalX, goalY).
    vector<pair<int,int>> computePath(int startX, int startY, int goalX, int goalY) {
        struct Node {
            int x, y;
            float g, h, f;
            int parentX, parentY;
        };
        auto heuristic = [&](int x, int y) {
            return static_cast<float>(abs(x - goalX) + abs(y - goalY));
        };
        vector<vector<bool>> closed(gridHeight, vector<bool>(gridWidth, false));
        vector<vector<Node>> nodeDetails(gridHeight, vector<Node>(gridWidth));
        auto cmp = [](const Node &a, const Node &b) {
            return a.f > b.f;
        };
        priority_queue<Node, vector<Node>, decltype(cmp)> open(cmp);
        
        // Initialize the start node.
        Node startNode;
        startNode.x = startX; startNode.y = startY;
        startNode.g = 0.0f; startNode.h = heuristic(startX, startY);
        startNode.f = startNode.h;
        startNode.parentX = -1; startNode.parentY = -1;
        open.push(startNode);
        nodeDetails[startY][startX] = startNode;
        
        bool pathFound = false;
        while (!open.empty()) {
            Node current = open.top();
            open.pop();
            int cx = current.x, cy = current.y;
            if (closed[cy][cx])
                continue;
            closed[cy][cx] = true;
            if (cx == goalX && cy == goalY) {
                pathFound = true;
                break;
            }
            // Check neighbors (up, down, left, right)
            int dx[4] = {0, 0, -1, 1};
            int dy[4] = {-1, 1, 0, 0};
            for (int dir = 0; dir < 4; dir++) {
                int nx = cx + dx[dir];
                int ny = cy + dy[dir];
                if (nx < 0 || nx >= gridWidth || ny < 0 || ny >= gridHeight)
                    continue;
                if (grid[ny][nx] == 1) continue; // obstacle
                if (closed[ny][nx]) continue;
                float gNew = current.g + 1.0f;
                if (gNew < nodeDetails[ny][nx].g || nodeDetails[ny][nx].f == 0) {
                    nodeDetails[ny][nx].x = nx;
                    nodeDetails[ny][nx].y = ny;
                    nodeDetails[ny][nx].g = gNew;
                    nodeDetails[ny][nx].h = heuristic(nx, ny);
                    nodeDetails[ny][nx].f = nodeDetails[ny][nx].g + nodeDetails[ny][nx].h;
                    nodeDetails[ny][nx].parentX = cx;
                    nodeDetails[ny][nx].parentY = cy;
                    open.push(nodeDetails[ny][nx]);
                }
            }
        }
        vector<pair<int,int>> path;
        if (!pathFound)
            return path;
        // Backtrack from goal to start
        int x = goalX, y = goalY;
        while (!(x == startX && y == startY)) {
            path.push_back({x,y});
            int px = nodeDetails[y][x].parentX;
            int py = nodeDetails[y][x].parentY;
            x = px; y = py;
        }
        reverse(path.begin(), path.end());
        return path;
    }
    
    void update() override {
        lock_guard<mutex> lock(unitMutex);
        // For each unit that is moving, follow the path.
        for (auto &unit : units) {
            if (unit.moving && !unit.path.empty()) {
                auto nextStep = unit.path.front();
                unit.path.erase(unit.path.begin());
                unit.x = nextStep.first;
                unit.y = nextStep.second;
                if (unit.path.empty())
                    unit.moving = false;
                logEvent("Unit " + unit.name + " moved to (" + to_string(unit.x) + "," + to_string(unit.y) + ")");
            }
        }
    }
    
    void shutdown() override {
        lock_guard<mutex> lock(unitMutex);
        units.clear();
    }
    
    void addUnit(const Unit &unit) {
        lock_guard<mutex> lock(unitMutex);
        units.push_back(unit);
    }
    
    void setDestinationForUnit(int index, int destX, int destY) {
        lock_guard<mutex> lock(unitMutex);
        if (index < 0 || index >= units.size())
            return;
        Unit &unit = units[index];
        unit.destX = destX;
        unit.destY = destY;
        unit.path = computePath(unit.x, unit.y, destX, destY);
        unit.moving = !unit.path.empty();
    }
    
    void printUnits() {
        lock_guard<mutex> lock(unitMutex);
        cout << "== Unit Status ==" << endl;
        for (const auto &unit : units) {
            cout << unit.name << " Health:" << unit.health
                 << " Pos: (" << unit.x << "," << unit.y << ")"
                 << " Dest: (" << unit.destX << "," << unit.destY << ")"
                 << endl;
        }
    }
};

//------------------------------------------------------------------------------
// CombatModule - Simulates Combat (Placeholder)
//------------------------------------------------------------------------------
class CombatModule : public Module {
    mutex combatMutex;
public:
    bool init() override {
        // EXPAND: Initialize combat systems, load weapons, etc.
        return true;
    }
    void update() override {
        lock_guard<mutex> lock(combatMutex);
        // EXPAND: Implement combat logic between units (use proximity, damage calculations, etc.).
    }
    void shutdown() override {
        // EXPAND: Cleanup combat-related resources.
    }
};

//------------------------------------------------------------------------------
// EconomyModule - Tracks Economy Value
//------------------------------------------------------------------------------
class EconomyModule : public Module {
    int economyValue;
    mutex econMutex;
public:
    bool init() override {
        economyValue = 1000;
        return true;
    }
    void update() override {
        lock_guard<mutex> lock(econMutex);
        // EXPAND: Include complex economic models, resource generation and consumption.
        economyValue += 1; // Simple increase.
        if (economyValue % 100 == 0)
            logEvent("Economy value updated: " + to_string(economyValue));
    }
    void shutdown() override {
        // EXPAND: Save/cleanup economic state.
    }
};

//------------------------------------------------------------------------------
// ChatModule - Handles Text Chat via Console Input in a Separate Thread
//------------------------------------------------------------------------------
class ChatModule : public Module {
    vector<string> messages;
    mutex chatMutex;
    thread inputThread;
    atomic<bool> moduleRunning;
public:
    bool init() override {
        moduleRunning.store(true);
        inputThread = thread([this]() { this->chatInput(); });
        return true;
    }
    void chatInput() {
        cout << "Chat Module Initialized. Type messages (type 'exit' to finish chat):" << endl;
        string line;
        while (moduleRunning.load()) {
            if (getline(cin, line)) {
                if (line == "exit") {
                    moduleRunning.store(false);
                    break;
                }
                // EXPAND: Replace with network transmission if needed.
                addMessage(line);
            } else {
                this_thread::sleep_for(chrono::milliseconds(50));
            }
        }
    }
    void addMessage(const string &msg) {
        lock_guard<mutex> lock(chatMutex);
        messages.push_back(msg);
    }
    void update() override {
        lock_guard<mutex> lock(chatMutex);
        if (!messages.empty()) {
            cout << "=== Chat Messages ===" << endl;
            for (auto &msg : messages) {
                cout << msg << endl;
            }
            cout << "=====================" << endl;
            messages.clear();
        }
    }
    void shutdown() override {
        moduleRunning.store(false);
        if (inputThread.joinable())
            inputThread.join();
    }
};

//------------------------------------------------------------------------------
// MiscModule - Additional Diagnostics, Event Scheduling, etc.
//------------------------------------------------------------------------------
class MiscModule : public Module {
public:
    bool init() override {
        return true;
    }
    void update() override {
        // EXPAND: Add diagnostics, error handling, event scheduling, etc.
        static int counter = 0;
        counter++;
        if (counter % 250 == 0)
            logEvent("MiscModule Diagnostics: All systems nominal.");
    }
    void shutdown() override {}
};

//------------------------------------------------------------------------------
// GameEngine - Orchestrates All Modules
//------------------------------------------------------------------------------
class GameEngine {
public:
    vector<Module*> modules; // Made public for ease of access for demonstration.
private:
    atomic<bool> engineRunning;
    thread mainLoopThread;
    mutex engineMutex;
public:
    GameEngine() { engineRunning.store(false); }
    
    bool init() {
        // Instantiate modules.
        modules.push_back(new UnitModule());
        modules.push_back(new CombatModule());
        modules.push_back(new EconomyModule());
        modules.push_back(new ChatModule());
        modules.push_back(new MiscModule());
        // Initialize each module.
        for (Module* mod : modules) {
            if (!mod->init()) {
                logEvent("Error initializing a module.");
                return false;
            }
        }
        engineRunning.store(true);
        return true;
    }
    
    void run() {
        mainLoopThread = thread(&GameEngine::mainLoop, this);
    }
    
    void mainLoop() {
        int iteration = 0;
        while (engineRunning.load()) {
            for (Module* mod : modules)
                mod->update();
            // Call UnitModule printing status every 100 iterations.
            if (iteration % 100 == 0) {
                UnitModule* um = dynamic_cast<UnitModule*>(modules[0]);
                if (um) {
                    um->printUnits();
                }
            }
            this_thread::sleep_for(chrono::milliseconds(33)); // ~30 FPS
            iteration++;
        }
    }
    
    void shutdown() {
        engineRunning.store(false);
        if (mainLoopThread.joinable())
            mainLoopThread.join();
        for (Module* mod : modules) {
            mod->shutdown();
            delete mod;
        }
        modules.clear();
    }
};

//------------------------------------------------------------------------------
// Main Function
//------------------------------------------------------------------------------
int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    logEvent("Game Engine Starting...");
    
    GameEngine engine;
    if (!engine.init()) {
        logEvent("Engine initialization failed.");
        return 1;
    }
    
    // Example: Set a destination for the first unit.
    UnitModule* unitMod = dynamic_cast<UnitModule*>(engine.modules[0]);
    if (unitMod) {
        unitMod->setDestinationForUnit(0, 15, 15);
    }
    
    engine.run();
    
    // Let the engine run for 10 seconds.
    this_thread::sleep_for(chrono::seconds(10));
    
    engine.shutdown();
    
    logEvent("Game Engine Terminated.");
    return 0;
}
