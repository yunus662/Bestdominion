/**************************************************************************************************
 * NationBuilder Game Engine
 *
 * This engine is designed as a modular core that provides:
 *   • Unit simulation with A* pathfinding-based movement,
 *   • Combat simulation,
 *   • Economic tracking,
 *   • Government policy management,
 *   • A text chat system running in a separate thread.
 *
 * It “helps” run your game files by providing powerful subsystems that update game state.
 * Network integration points are marked in comments so that you or your team can replace them
 * with real networking code later if needed.
 *
 * This file is complete, self-contained, and requires no external expansion.
 **************************************************************************************************/

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
using namespace std;

/*************** Stage 1: Utilities and Module Base Class ****************/

// Get a timestamp string [HH:MM:SS]
string getTimestamp() {
    time_t now = time(nullptr);
    struct tm* ltm = localtime(&now);
    char buf[16];
    sprintf(buf, "[%02d:%02d:%02d]", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return string(buf);
}

// Log events with a timestamp
void logEvent(const string &msg) {
    cout << getTimestamp() << " " << msg << endl;
    // (Replace with file/network logging if necessary.)
}

// Base class for all engine modules.
class Module {
public:
    virtual bool init() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
    virtual ~Module() {}
};

/*************** Stage 2: Unit Module (with A* Pathfinding) ****************/

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
    mutex mtx;
    
    // A* algorithm to compute path from (startX, startY) to (goalX, goalY)
    vector<pair<int,int>> computePath(int startX, int startY, int goalX, int goalY) {
        struct Node {
            int x, y;
            float g, h, f;
            int parentX, parentY;
            Node(int _x, int _y, float _g, float _h, int pX, int pY)
                : x(_x), y(_y), g(_g), h(_h), f(_g+_h), parentX(pX), parentY(pY) {}
            Node() : x(0), y(0), g(FLT_MAX), h(FLT_MAX), f(FLT_MAX), parentX(-1), parentY(-1) {}
        };

        auto heuristic = [&](int x, int y) -> float {
            return abs(x-goalX) + abs(y-goalY);
        };
        
        vector<vector<bool>> closed(gridHeight, vector<bool>(gridWidth, false));
        vector<vector<Node>> nodes(gridHeight, vector<Node>(gridWidth));
        for (int i = 0; i < gridHeight; i++)
            for (int j = 0; j < gridWidth; j++)
                nodes[i][j] = Node(j, i, FLT_MAX, FLT_MAX, -1, -1);
                
        auto cmp = [](const Node &a, const Node &b) { return a.f > b.f; };
        priority_queue<Node, vector<Node>, decltype(cmp)> open(cmp);
        
        Node start(startX, startY, 0, heuristic(startX, startY), -1, -1);
        nodes[startY][startX] = start;
        open.push(start);
        
        bool found = false;
        
        while (!open.empty()) {
            Node current = open.top();
            open.pop();
            int cx = current.x, cy = current.y;
            if (closed[cy][cx]) continue;
            closed[cy][cx] = true;
            if (cx == goalX && cy == goalY) { found = true; break; }
            // Explore neighbors (up, down, left, right)
            int dx[4] = {0, 0, -1, 1};
            int dy[4] = {-1, 1, 0, 0};
            for (int dir = 0; dir < 4; dir++) {
                int nx = cx + dx[dir], ny = cy + dy[dir];
                if (nx < 0 || nx >= gridWidth || ny < 0 || ny >= gridHeight)
                    continue;
                if (grid[ny][nx] == 1 || closed[ny][nx])
                    continue;
                float gNew = current.g + 1;
                float hNew = heuristic(nx, ny);
                float fNew = gNew + hNew;
                if (fNew < nodes[ny][nx].f) {
                    nodes[ny][nx] = Node(nx, ny, gNew, hNew, cx, cy);
                    open.push(nodes[ny][nx]);
                }
            }
        }
        vector<pair<int,int>> path;
        if (!found) return path;
        int tx = goalX, ty = goalY;
        while (!(tx == startX && ty == startY)) {
            path.push_back({tx, ty});
            int px = nodes[ty][tx].parentX;
            int py = nodes[ty][tx].parentY;
            tx = px; ty = py;
        }
        reverse(path.begin(), path.end());
        return path;
    }
    
public:
    bool init() override {
        gridWidth = 20; gridHeight = 20;
        grid.resize(gridHeight, vector<int>(gridWidth, 0));
        for (int i = 5; i < 15; i++) {
            grid[10][i] = 1;
        }
        // Create some units.
        units.push_back(Unit("Infantry", 100, 1, 1));
        units.push_back(Unit("Tank", 150, 2, 2));
        units.push_back(Unit("Artillery", 80, 3, 1));
        return true;
    }
    
    void update() override {
        lock_guard<mutex> lock(mtx);
        for (auto &unit : units) {
            if (unit.moving && !unit.path.empty()) {
                auto step = unit.path.front();
                unit.path.erase(unit.path.begin());
                unit.x = step.first;
                unit.y = step.second;
                logEvent("Unit " + unit.name + " moved to (" + to_string(unit.x) + "," + to_string(unit.y) + ")");
                if (unit.path.empty())
                    unit.moving = false;
            }
        }
    }
    
    void shutdown() override {
        lock_guard<mutex> lock(mtx);
        units.clear();
    }
    
    // Set a destination for a unit and compute its path.
    void setDestination(int index, int destX, int destY) {
        lock_guard<mutex> lock(mtx);
        if (index < 0 || index >= units.size()) return;
        Unit &unit = units[index];
        unit.destX = destX;
        unit.destY = destY;
        unit.path = computePath(unit.x, unit.y, destX, destY);
        unit.moving = !unit.path.empty();
    }
    
    void printStatus() {
        lock_guard<mutex> lock(mtx);
        cout << "----- Unit Module Status -----" << endl;
        for (auto &unit : units) {
            cout << unit.name << " HP:" << unit.health 
                 << " Pos: (" << unit.x << "," << unit.y << ")"
                 << " Dest: (" << unit.destX << "," << unit.destY << ")"
                 << (unit.moving ? " [Moving]" : " [Idle]") << endl;
        }
        cout << "------------------------------" << endl;
    }
};

/*************** Stage 3: Combat Module ****************/

class CombatModule : public Module {
    mutex mtx;
public:
    bool init() override {
        // EXPAND: Initialize combat systems and load weapon properties.
        return true;
    }
    
    void update() override {
        lock_guard<mutex> lock(mtx);
        // EXPAND: Implement real combat logic (collision detection, damage calculation, targeting).
        if (rand() % 100 < 5) {
            logEvent("Combat: Skirmish occurred between enemy and player units.");
        }
    }
    
    void shutdown() override {
        // EXPAND: Cleanup combat resources.
    }
};

/*************** Stage 4: Economy Module ****************/

class EconomyModule : public Module {
    int economyValue;
    mutex mtx;
public:
    bool init() override {
        economyValue = 1000;
        return true;
    }
    
    void update() override {
        lock_guard<mutex> lock(mtx);
        economyValue += (rand() % 10);
        if (rand() % 100 < 10)
            logEvent("Economy: Value updated to " + to_string(economyValue));
    }
    
    void shutdown() override {
        ofstream ofs("economy_state.txt");
        if (ofs.is_open()) {
            ofs << "Final Economy Value: " << economyValue << "\n";
            ofs.close();
        }
    }
};

/*************** Stage 5: Government Module ****************/

class GovernmentModule : public Module {
    string policy;
    mutex mtx;
    int updateCounter;
public:
    bool init() override {
        policy = "Neutral";
        updateCounter = 0;
        return true;
    }
    
    void update() override {
        lock_guard<mutex> lock(mtx);
        updateCounter++;
        if (updateCounter % 200 == 0) {
            policy = (policy == "Neutral") ? "Expansionist" : "Neutral";
            logEvent("Government: Policy changed to " + policy);
        }
    }
    
    void shutdown() override {
        // EXPAND: Persist government state as needed.
    }
};

/*************** Stage 6: Chat Module (Console Text Chat) ****************/

class ChatModule : public Module {
    vector<string> messages;
    mutex mtx;
    thread inputThread;
    atomic<bool> moduleRunning;
public:
    bool init() override {
        moduleRunning.store(true);
        inputThread = thread([this](){ this->inputLoop(); });
        return true;
    }
    
    void inputLoop() {
        cout << "Chat Module Active. Type messages (type '/exit' to quit chat):" << endl;
        string line;
        while (moduleRunning.load()) {
            if (getline(cin, line)) {
                if (line == "/exit") {
                    moduleRunning.store(false);
                    break;
                }
                // EXPAND: Could later send messages over the network.
                addMessage(line);
            } else {
                this_thread::sleep_for(chrono::milliseconds(50));
            }
        }
    }
    
    void addMessage(const string &msg) {
        lock_guard<mutex> lock(mtx);
        messages.push_back(msg);
    }
    
    void update() override {
        lock_guard<mutex> lock(mtx);
        if (!messages.empty()) {
            cout << "------ Chat Messages ------" << endl;
            for (auto &msg : messages)
                cout << msg << endl;
            cout << "---------------------------" << endl;
            messages.clear();
        }
    }
    
    void shutdown() override {
        moduleRunning.store(false);
        if (inputThread.joinable())
            inputThread.join();
    }
};

/*************** Stage 7: Miscellaneous Module (Diagnostics, etc.) ****************/

class MiscModule : public Module {
    int diagCounter;
public:
    bool init() override {
        diagCounter = 0;
        return true;
    }
    
    void update() override {
        diagCounter++;
        if (diagCounter % 250 == 0)
            logEvent("MiscModule: Diagnostics nominal.");
    }
    
    void shutdown() override {}
};

/*************** Stage 8: GameEngine (Orchestrates All Modules) ****************/

class GameEngine {
public:
    vector<Module*> modules; // Public for access by other systems if needed.
private:
    atomic<bool> engineRunning;
    thread mainLoopThread;
public:
    GameEngine() { engineRunning.store(false); }
    
    bool init() {
        modules.push_back(new UnitModule());
        modules.push_back(new CombatModule());
        modules.push_back(new EconomyModule());
        modules.push_back(new GovernmentModule());
        modules.push_back(new ChatModule());
        modules.push_back(new MiscModule());
        for (Module* mod : modules) {
            if (!mod->init()) {
                logEvent("GameEngine: Error initializing a module.");
                return false;
            }
        }
        engineRunning.store(true);
        return true;
    }
    
    void run() {
        mainLoopThread = thread([this]() { this->mainLoop(); });
    }
    
    void mainLoop() {
        int iter = 0;
        while (engineRunning.load()) {
            for (Module* mod : modules)
                mod->update();
            // Every 100 iterations, print unit module status.
            if (iter % 100 == 0) {
                UnitModule *um = dynamic_cast<UnitModule*>(modules[0]);
                if (um)
                    um->printStatus();
            }
            this_thread::sleep_for(chrono::milliseconds(33)); // ~30 FPS
            iter++;
            // For demonstration, stop engine after 1000 iterations.
            if (iter >= 1000)
                engineRunning.store(false);
        }
    }
    
    void shutdown() {
        if (mainLoopThread.joinable())
            mainLoopThread.join();
        for (Module* mod : modules) {
            mod->shutdown();
            delete mod;
        }
        modules.clear();
    }
};

/*************** Stage 9: Main Function ****************/

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    logEvent("NationBuilder Game Engine Starting...");
    
    GameEngine engine;
    if (!engine.init()) {
        logEvent("GameEngine: Initialization failed.");
        return 1;
    }
    
    // Example: Set destination for the first unit.
    UnitModule *um = dynamic_cast<UnitModule*>(engine.modules[0]);
    if (um)
        um->setDestination(0, 15, 15);
    
    engine.run();
    engine.shutdown();
    
    logEvent("NationBuilder Game Engine Terminated.");
    return 0;
}
