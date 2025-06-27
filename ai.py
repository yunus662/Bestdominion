#!/usr/bin/env python3
"""
ai.py
=====
A production‑quality AI module for the Conqueror Engine.
"""

import math
import random
import logging
import time
from collections import deque

# -----------------------------------------------------------------------------
# Logging Configuration
# -----------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(levelname)s] %(asctime)s - %(message)s',
    datefmt='%H:%M:%S'
)

# -----------------------------------------------------------------------------
# Behavior Tree Nodes
# -----------------------------------------------------------------------------

class Node:
    def __init__(self, name="Node"):
        self.name = name

    def run(self):
        raise NotImplementedError

    def reset(self):
        pass

class Composite(Node):
    def __init__(self, name="Composite"):
        super().__init__(name)
        self.children = []

    def add_child(self, child):
        self.children.append(child)

    def reset(self):
        for child in self.children:
            child.reset()

class Sequence(Composite):
    def run(self):
        for child in self.children:
            if not child.run():
                logging.debug(f"Sequence {self.name}: '{child.name}' failed")
                return False
        logging.debug(f"Sequence {self.name}: succeeded")
        return True

class Selector(Composite):
    def run(self):
        for child in self.children:
            if child.run():
                logging.debug(f"Selector {self.name}: '{child.name}' succeeded")
                return True
        logging.debug(f"Selector {self.name}: failed")
        return False

class Inverter(Node):
    def __init__(self, child, name="Inverter"):
        super().__init__(name)
        self.child = child

    def run(self):
        result = not self.child.run()
        logging.debug(f"Inverter {self.name}: result {result}")
        return result

    def reset(self):
        self.child.reset()

class Action(Node):
    def __init__(self, action_fn, name="Action"):
        super().__init__(name)
        self.action_fn = action_fn

    def run(self):
        result = self.action_fn()
        logging.debug(f"Action {self.name}: {result}")
        return result

class Condition(Node):
    def __init__(self, condition_fn, name="Condition"):
        super().__init__(name)
        self.condition_fn = condition_fn

    def run(self):
        result = self.condition_fn()
        logging.debug(f"Condition {self.name}: {result}")
        return result

class BehaviorTree:
    def __init__(self, root):
        self.root = root

    def run(self):
        return self.root.run()

    def reset(self):
        self.root.reset()

# -----------------------------------------------------------------------------
# A* Pathfinding
# -----------------------------------------------------------------------------

class AStarPathfinder:
    def __init__(self, grid):
        self.grid = grid
        self.rows = len(grid)
        self.cols = len(grid[0]) if self.rows > 0 else 0

    def heuristic(self, a, b):
        return abs(a[0] - b[0]) + abs(a[1] - b[1])

    def in_bounds(self, node):
        x, y = node
        return 0 <= x < self.cols and 0 <= y < self.rows

    def passable(self, node):
        x, y = node
        return self.grid[y][x] == 0

    def neighbors(self, node):
        x, y = node
        options = [(x+1, y), (x-1, y), (x, y+1), (x, y-1)]
        return [n for n in options if self.in_bounds(n) and self.passable(n)]

    def find_path(self, start, goal):
        import heapq
        frontier = []
        heapq.heappush(frontier, (0, start))
        came_from = {start: None}
        cost_so_far = {start: 0}

        while frontier:
            _, current = heapq.heappop(frontier)
            if current == goal:
                break
            for nxt in self.neighbors(current):
                new_cost = cost_so_far[current] + 1
                if nxt not in cost_so_far or new_cost < cost_so_far[nxt]:
                    cost_so_far[nxt] = new_cost
                    priority = new_cost + self.heuristic(goal, nxt)
                    heapq.heappush(frontier, (priority, nxt))
                    came_from[nxt] = current

        path = []
        current = goal
        while current is not None:
            path.append(current)
            current = came_from.get(current)
        path.reverse()

        if path and path[0] == start:
            logging.debug(f"Path found: {path}")
            return path
        logging.debug("No path found")
        return []

# -----------------------------------------------------------------------------
# Unit AI
# -----------------------------------------------------------------------------

class UnitAI:
    def __init__(self, unit_id, position):
        self.unit_id = unit_id
        self.position = position
        self.state = "idle"
        self.target = None
        self.path = []
        self.behavior_tree = None
        self.init_behavior_tree()

    def init_behavior_tree(self):
        def idle():
            self.state = "idle"
            logging.debug(f"Unit {self.unit_id} is idling at {self.position}")
            return True

        def has_target():
            return self.target is not None

        def move_to_target():
            if not self.target:
                return False
            dx, dy = self.target[0] - self.position[0], self.target[1] - self.position[1]
            self.position = (self.position[0] + dx * 0.1, self.position[1] + dy * 0.1)
            if math.hypot(dx, dy) < 1:
                self.position = self.target
                self.target = None
            self.state = "moving"
            return True

        idle_action = Action(idle, "Idle")
        move_action = Action(move_to_target, "MoveToTarget")
        condition_target = Condition(has_target, "HasTarget")

        move_sequence = Sequence("MoveSequence")
        move_sequence.add_child(condition_target)
        move_sequence.add_child(move_action)

        root = Selector("UnitRoot")
        root.add_child(move_sequence)
        root.add_child(idle_action)

        self.behavior_tree = BehaviorTree(root)

    def update(self):
        if self.behavior_tree:
            self.behavior_tree.run()

    def set_target(self, target):
        self.target = target
        logging.info(f"Unit {self.unit_id} target set to {target}")

    def get_state(self):
        return self.state

# -----------------------------------------------------------------------------
# AI Manager
# -----------------------------------------------------------------------------

class AIManager:
    def __init__(self):
        self.unit_ais = {}
        self.next_unit_id = 1

    def register_unit(self, position):
        uid = self.next_unit_id
        self.unit_ais[uid] = UnitAI(uid, position)
        self.next_unit_id += 1
        logging.info(f"Registered unit {uid} at {position}")
        return uid

    def assign_target_to_unit(self, unit_id, target):
        unit = self.unit_ais.get(unit_id)
        if unit:
            unit.set_target(target)
        else:
            logging.error(f"Unit {unit_id} not found")

    def update(self):
        for unit in self.unit_ais.values():
            unit.update()

    def plan_group_strategy(self):
        for unit in self.unit_ais.values():
            random_target = (random.uniform(0, 100), random.uniform(0, 100))
            unit.set_target(random_target)

    def reset_all(self):
        for unit in self.unit_ais.values():
            unit.behavior_tree.reset()

# -----------------------------------------------------------------------------
# Utility Functions
# -----------------------------------------------------------------------------

def vector_add(v1, v2):
    return (v1[0] + v2[0], v1[1] + v2[1])

def vector_subtract(v1, v2):
    return (v1[0] - v2[0], v1[1] - v2[1])

def vector_length(v):
    return math.sqrt(v[0] ** 2 + v[1] ** 2)

def vector_normalize(v):
    l = vector_length(v)
    return (0, 0) if l == 0 else (v[0]/l, v[1]/l)

# -----------------------------------------------------------------------------
# Simulation for Testing
# -----------------------------------------------------------------------------

def simulate_ai_update():
    ai_manager = AIManager()
    for _ in range(10):
        pos = (random.uniform(0, 100), random.uniform(0, 100))
        ai_manager.register_unit(pos)

    for i in range(100):
        logging.info(f"Simulation iteration {i+1}")
        if i % 10 == 0:
            ai_manager.plan_group_strategy()
        ai_manager.update()
        time.sleep(0.05)

if __name__ == '__main__':
    logging.info("AI Simulation Start")
    simulate_ai_update()
    logging.info("AI Simulation End")
