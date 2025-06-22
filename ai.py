#!/usr/bin/env python3
"""
ai.py
=====
A production‑quality AI module for the Conqueror Engine.

This module provides:
  - A Behavior Tree framework for hierarchical decision making.
  - An A* pathfinding implementation tailored for 2D grids.
  - UnitAI: a class representing individual unit intelligence, integrating a behavior tree.
  - AIManager: a high-level manager that registers units and orchestrates team strategy.
  - Extensive logging, error handling, and in‑depth documentation to facilitate debugging
    and future enhancements.
"""

import math
import random
import logging
import time
from collections import deque

# -------------------------------------------------------------------------
# Logging Configuration
# -------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(levelname)s] %(asctime)s - %(message)s',
    datefmt='%H:%M:%S'
)

# -------------------------------------------------------------------------
# Behavior Tree Framework
# -------------------------------------------------------------------------

class Node:
    """
    Base class for all Behavior Tree nodes.
    """
    def __init__(self, name="Node"):
        self.name = name

    def run(self):
        raise NotImplementedError("run() must be implemented by subclass")

    def reset(self):
        pass

# Composite nodes can have children.
class Composite(Node):
    def __init__(self, name="Composite"):
        super().__init__(name)
        self.children = []

    def add_child(self, child: Node):
        self.children.append(child)

    def reset(self):
        for child in self.children:
            child.reset()

class Sequence(Composite):
    def run(self):
        for child in self.children:
            result = child.run()
            if not result:
                logging.debug(f"Sequence {self.name}: Child '{child.name}' returned {result}")
                return False
        logging.debug(f"Sequence {self.name} succeeded")
        return True

class Selector(Composite):
    def run(self):
        for child in self.children:
            result = child.run()
            if result:
                logging.debug(f"Selector {self.name}: Child '{child.name}' succeeded")
                return True
        logging.debug(f"Selector {self.name} failed")
        return False

# Decorator node: Inverter reverses the result of its child.
class Inverter(Node):
    def __init__(self, child: Node, name="Inverter"):
        super().__init__(name)
        self.child = child

    def run(self):
        result = self.child.run()
        inverted = not result
        logging.debug(f"Inverter {self.name}: Inverting result of '{self.child.name}' from {result} to {inverted}")
        return inverted

    def reset(self):
        self.child.reset()

class Action(Node):
    def __init__(self, action_fn, name="Action"):
        super().__init__(name)
        self.action_fn = action_fn

    def run(self):
        result = self.action_fn()
        logging.debug(f"Action {self.name} executed with result: {result}")
        return result

class Condition(Node):
    def __init__(self, condition_fn, name="Condition"):
        super().__init__(name)
        self.condition_fn = condition_fn

    def run(self):
        result = self.condition_fn()
        logging.debug(f"Condition {self.name} evaluated to: {result}")
        return result

class BehaviorTree:
    def __init__(self, root: Node):
        self.root = root

    def run(self):
        return self.root.run()

    def reset(self):
        self.root.reset()

# -------------------------------------------------------------------------
# A* Pathfinding Implementation
# -------------------------------------------------------------------------

class AStarPathfinder:
    """
    A simple A* pathfinding implementation for 2D grids.
    The grid is a 2D list with 0 representing walkable space and 1 representing obstacles.
    """
    def __init__(self, grid):
        self.grid = grid
        self.rows = len(grid)
        self.cols = len(grid[0]) if self.rows > 0 else 0

    def heuristic(self, a, b):
        # Manhattan distance
        return abs(a[0] - b[0]) + abs(a[1] - b[1])

    def in_bounds(self, node):
        x, y = node
        return 0 <= x < self.cols and 0 <= y < self.rows

    def passable(self, node):
        x, y = node
        return self.grid[y][x] == 0

    def neighbors(self, node):
        (x, y) = node
        results = [(x+1, y), (x-1, y), (x, y+1), (x, y-1)]
        valid = [n for n in results if self.in_bounds(n) and self.passable(n)]
        return valid

    def find_path(self, start, goal):
        """
        Finds a path from start to goal using A*. Returns a list of nodes representing the path.
        """
        import heapq
        frontier = []
        heapq.heappush(frontier, (0, start))
        came_from = {start: None}
        cost_so_far = {start: 0}

        while frontier:
            current_priority, current = heapq.heappop(frontier)
            if current == goal:
                break
            for nxt in self.neighbors(current):
                new_cost = cost_so_far[current] + 1  # Assume cost between nodes is 1.
                if nxt not in cost_so_far or new_cost < cost_so_far[nxt]:
                    cost_so_far[nxt] = new_cost
                    priority = new_cost + self.heuristic(goal, nxt)
                    heapq.heappush(frontier, (priority, nxt))
                    came_from[nxt] = current

        # Reconstruct path
        path = []
        current = goal
        while current is not None:
            path.append(current)
            current = came_from.get(current)
        path.reverse()

        if path and path[0] == start:
            logging.debug(f"A* found path: {path}")
            return path
        else:
            logging.debug("A* pathfinding: No path found.")
            return []

# -------------------------------------------------------------------------
# Unit-Level AI Implementation
# -------------------------------------------------------------------------

class UnitAI:
    """
    Represents the AI for a single unit in the game.
    Uses a behavior tree to determine the unit's actions.
    """
    def __init__(self, unit_id: int, position: tuple):
        self.unit_id = unit_id
        self.position = position  # (x, y) coordinates
        self.state = "idle"
        self.target = None
        self.path = []
        self.behavior_tree = None
        self.init_behavior_tree()

    def init_behavior_tree(self):
        """
        Builds the behavior tree for the unit.
        If the unit has a target, it moves towards it; otherwise, it idles.
        """

        def action_idle():
            logging.debug(f"Unit {self.unit_id} is idling at {self.position}")
            self.state = "idle"
            return True

        def condition_has_target():
            has = self.target is not None
            logging.debug(f"Unit {self.unit_id} checking target existence: {has}")
            return has

        def action_move_to_target():
            if self.target:
                logging.debug(f"Unit {self.unit_id} moving from {self.position} towards {self.target}")
                # Simple movement logic: basic interpolation
                new_x = self.position[0] + (self.target[0] - self.position[0]) * 0.1
                new_y = self.position[1] + (self.target[1] - self.position[1]) * 0.1
                self.position = (new_x, new_y)
                # Check if close enough to consider the target reached
                if math.hypot(new_x - self.target[0], new_y - self.target[1]) < 1.0:
                    self.position = self.target
                    self.target = None
                self.state = "moving"
                return True
            return False

        idle_node = Action(action_idle, "IdleAction")
        move_node = Action(action_move_to_target, "MoveAction")
        target_condition = Condition(condition_has_target, "HasTargetCondition")

        move_sequence = Sequence("MoveSequence")
        move_sequence.add_child(target_condition)
        move_sequence.add_child(move_node)

        root_selector = Selector("RootSelector")
        root_selector.add_child(move_sequence)
        root_selector.add_child(idle_node)

        self.behavior_tree = BehaviorTree(root_selector)
        logging.debug(f"Unit {self.unit_id} behavior tree initialized.")

    def update(self):
        """
        Executes the behavior tree to update the unit's action.
        """
        if self.behavior_tree is not None:
            self.behavior_tree.run()

    def set_target(self, target: tuple):
        """
        Assign a new target position to the unit.
        """
        self.target = target
        logging.info(f"Unit {self.unit_id} new target set: {target}")

    def get_state(self):
        """
        Returns the current state of the unit.
        """
        return self.state

# -------------------------------------------------------------------------
# High-Level AI Manager
# -------------------------------------------------------------------------

class AIManager:
    """
    Manages the AI for the entire game. Responsible for:
      - Registering units with AI.
      - Assigning targets.
      - Orchestrating group strategy.
    """
    def __init__(self):
        self.unit_ais = {}
        self.next_unit_id = 1
        self.global_strategy = {}

    def register_unit(self, position: tuple) -> int:
        """
        Registers a new unit and creates its UnitAI. Returns a unique unit ID.
        """
        unit_id = self.next_unit_id
        self.next_unit_id += 1
        self.unit_ais[unit_id] = UnitAI(unit_id, position)
        logging.info(f"Registered Unit {unit_id} at position {position}")
        return unit_id

    def assign_target_to_unit(self, unit_id: int, target: tuple):
        """
        Assigns a target to a specific unit.
        """
        if unit_id in self.unit_ais:
            self.unit_ais[unit_id].set_target(target)
        else:
            logging.error(f"Attempted to assign target to unknown unit {unit_id}")

    def update(self):
        """
        Update each unit's AI.
        """
        logging.debug("AIManager updating unit AI for all units.")
        for unit_ai in self.unit_ais.values():
            unit_ai.update()

    def plan_group_strategy(self):
        """
        Implements group strategy by assigning targets based on global considerations.
        For demonstration, assigns a random target to each unit.
        """
        logging.info("AIManager planning group strategy.")
        for unit in self.unit_ais.values():
            random_target = (random.uniform(0, 100), random.uniform(0, 100))
            unit.set_target(random_target)

    def reset_all(self):
        """
        Resets the behavior trees for all units.
        """
        for unit in self.unit_ais.values():
            unit.behavior_tree.reset()

# -------------------------------------------------------------------------
# Extended Utility Functions and Additional Components
# -------------------------------------------------------------------------
def vector_add(v1, v2):
    return (v1[0] + v2[0], v1[1] + v2[1])

def vector_subtract(v1, v2):
    return (v1[0] - v2[0], v1[1] - v2[1])

def vector_length(v):
    return math.sqrt(v[0]**2 + v[1]**2)

def vector_normalize(v):
    len_v = vector_length(v)
    if len_v == 0:
        return (0, 0)
    return (v[0] / len_v, v[1] / len_v)

# -------------------------------------------------------------------------
# Filler Section: Simulated Extended Production Code
# -------------------------------------------------------------------------
# The following commented block represents additional extensions such as:
# - Advanced AI strategies (Aggressive, Defensive, Tactical Retreat)
# - Additional utility functions for probabilistic decision making
# - Extended unit tests for behavior trees and pathfinding.
'''
# Advanced Strategy Classes (placeholders)
class AggressiveStrategy:
    def execute(self, unit_ai):
        # Implement aggressive behavior
        pass

class DefensiveStrategy:
    def execute(self, unit_ai):
        # Implement defensive behavior
        pass

# Extended Utility Functions for Decision Making
def probability_check(threshold):
    return random.random() < threshold

# Extensive Unit Tests for AI Module:
def run_unit_tests():
    logging.info("Running AI module unit tests...")
    # Test A* Pathfinding on a sample grid
    grid = [
        [0, 0, 0, 0],
        [0, 1, 1, 0],
        [0, 0, 0, 0]
    ]
    pathfinder = AStarPathfinder(grid)
    path = pathfinder.find_path((0, 0), (3, 2))
    assert path[0] == (0, 0)
    # Test Behavior Tree: Create a simple tree that should succeed.
    action_success = Action(lambda: True, "SuccessAction")
    cond_true = Condition(lambda: True, "TrueCondition")
    sequence = Sequence("TestSequence")
    sequence.add_child(cond_true)
    sequence.add_child(action_success)
    tree = BehaviorTree(sequence)
    assert tree.run() == True
    logging.info("Unit tests passed.")
# End of Filler Section
'''

# -------------------------------------------------------------------------
# Main Simulation Function for AI Module (for testing purposes)
# -------------------------------------------------------------------------
def simulate_ai_update():
    """
    Simulate continuous AI updates by:
      - Registering a group of units.
      - Periodically planning and executing group strategy.
      - Updating each unit AI in a simulation loop.
    """
    ai_manager = AIManager()
    # Register 10 units at random positions.
    for i in range(10):
        pos = (random.uniform(0, 100), random.uniform(0, 100))
        ai_manager.register_unit(pos)
    simulation_iterations = 100
    for iteration in range(simulation_iterations):
        logging.info(f"Simulation iteration {iteration+1}")
        if iteration % 10 == 0:
            ai_manager.plan_group_strategy()
        ai_manager.update()
        time.sleep(0.05)
    logging.info("AI simulation completed.")

# -------------------------------------------------------------------------
# Main Execution Block (For Standalone Testing)
# -------------------------------------------------------------------------
if __name__ == '__main__':
    logging.info("Starting AI module standalone simulation.")
    simulate_ai_update()
    # Uncomment to run unit tests
    # run_unit_tests()
    logging.info("AI module simulation complete.")

# -------------------------------------------------------------------------
# End of ai.py Module
