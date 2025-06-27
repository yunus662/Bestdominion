#!/usr/bin/env python3
"""
events.py
=========
Production‑Quality World Event System for Conqueror Engine

This module implements a robust event system that manages global and regional
in‑game events which affect nations economically, diplomatically, environmentally,
and militarily.

Features:
  - A class hierarchy for events:
      • BaseEvent: Common event properties and interface.
      • EconomicEvent, DiplomaticEvent, EnvironmentalEvent, MilitaryEvent: Specialized events.
  - A WorldEventManager to schedule, trigger, and resolve events.
  - Support for both scheduled events (with start time and duration) and random events.
  - Detailed logging and inline documentation for production‑level quality.
  
Integration:
  Other game systems (economy, diplomacy, military) can tie into the event effects
  by registering callback functions with the WorldEventManager.
"""

import logging
import random
import time
from datetime import datetime, timedelta

# -------------------------------------------------------------------------
# Logging Configuration
# -------------------------------------------------------------------------
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(asctime)s] [%(levelname)s] %(message)s',
    datefmt='%H:%M:%S'
)

# -------------------------------------------------------------------------
# Event Class Hierarchy
# -------------------------------------------------------------------------

class BaseEvent:
    """
    The base class for all world events.
    
    Attributes:
      event_id (int): Auto-assigned unique identifier.
      event_type (str): Type/category of the event.
      description (str): A concise description of the event.
      start_time (datetime): When the event begins.
      duration (timedelta): How long the event lasts.
      resolved (bool): Indicates if the event has been fully processed.
    """
    _id_counter = 1

    def __init__(self, event_type: str, description: str, duration_minutes: int):
        self.event_id = BaseEvent._id_counter
        BaseEvent._id_counter += 1
        self.event_type = event_type
        self.description = description
        self.start_time = datetime.now()
        self.duration = timedelta(minutes=duration_minutes)
        self.resolved = False
        logging.debug(f"Event created: {self}")

    def is_active(self) -> bool:
        """Check if the event is currently active."""
        now = datetime.now()
        active = now < self.start_time + self.duration
        logging.debug(f"Event {self.event_id} active: {active}")
        return active

    def apply_effect(self):
        """
        Apply the effect of the event.
        This method should be overridden in derived classes to enforce specific behaviors.
        """
        raise NotImplementedError("apply_effect must be implemented by subclasses.")

    def resolve(self):
        """
        Mark the event as resolved and perform any cleanup actions.
        """
        self.resolved = True
        logging.info(f"Event {self.event_id} resolved: {self.description}")

    def __str__(self):
        return f"[{self.event_id}] {self.event_type}: {self.description} (Duration: {self.duration})"
    
# Economic Event
class EconomicEvent(BaseEvent):
    """
    Represents an economic event that can impact in‑game resources and markets.
    """
    def __init__(self, description: str, duration_minutes: int, economic_impact: float):
        """
        economic_impact: A multiplier affecting resource production (e.g., 0.9 reduces production by 10%)
        """
        super().__init__("Economic", description, duration_minutes)
        self.economic_impact = economic_impact

    def apply_effect(self):
        # In production, this would adjust nation economic multipliers.
        logging.info(f"Applying Economic Event [{self.event_id}]: {self.description} with impact {self.economic_impact}")
        # Placeholder: Call external economy system with self.economic_impact here.
        self.resolve()

# Diplomatic Event
class DiplomaticEvent(BaseEvent):
    """
    Represents a diplomatic event that can alter international relations.
    """
    def __init__(self, description: str, duration_minutes: int, trust_delta: float, hostility_delta: float):
        """
        trust_delta: Change in trust (can be positive or negative)
        hostility_delta: Change in hostility (can be positive or negative)
        """
        super().__init__("Diplomatic", description, duration_minutes)
        self.trust_delta = trust_delta
        self.hostility_delta = hostility_delta

    def apply_effect(self):
        logging.info(f"Applying Diplomatic Event [{self.event_id}]: {self.description} "
                     f"(Trust Δ: {self.trust_delta}, Hostility Δ: {self.hostility_delta})")
        # Placeholder: Modify diplomatic relations via external diplomacy system.
        self.resolve()

# Environmental Event
class EnvironmentalEvent(BaseEvent):
    """
    Represents an environmental event (such as natural disasters) that can affect infrastructure and resources.
    """
    def __init__(self, description: str, duration_minutes: int, damage_factor: float):
        """
        damage_factor: A factor representing the severity of infrastructure damage.
        """
        super().__init__("Environmental", description, duration_minutes)
        self.damage_factor = damage_factor

    def apply_effect(self):
        logging.info(f"Applying Environmental Event [{self.event_id}]: {self.description} with damage factor {self.damage_factor}")
        # Placeholder: Interface with infrastructure or resource systems to apply damage.
        self.resolve()

# Military Event
class MilitaryEvent(BaseEvent):
    """
    Represents a military event (e.g., skirmishes, battles, or surprise attacks) in the world.
    """
    def __init__(self, description: str, duration_minutes: int, combat_modifier: float):
        """
        combat_modifier: A factor that modifies combat effectiveness (could buff or nerf units temporarily).
        """
        super().__init__("Military", description, duration_minutes)
        self.combat_modifier = combat_modifier

    def apply_effect(self):
        logging.info(f"Applying Military Event [{self.event_id}]: {self.description} with combat modifier {self.combat_modifier}")
        # Placeholder: Trigger combat system modulations via external combat module.
        self.resolve()

# -------------------------------------------------------------------------
# WorldEventManager Class
# -------------------------------------------------------------------------
class WorldEventManager:
    """
    Manages all world events, handling scheduling, activation, and resolution.
    
    Capabilities:
      - Schedule new events (either random or predefined).
      - Update events over time.
      - Trigger events whose start time has arrived.
      - Remove or archive events once they are resolved.
    """
    def __init__(self):
        self.events = []
        logging.info("WorldEventManager initialized.")

    def schedule_event(self, event: BaseEvent):
        """
        Add a new event to the schedule.
        """
        self.events.append(event)
        logging.info(f"Scheduled new event: {event}")

    def update_events(self):
        """
        Process events: trigger events that are active, and remove resolved or expired events.
        """
        for event in self.events[:]:
            if not event.resolved:
                if event.is_active():
                    logging.debug(f"Event {event.event_id} is active. Applying effect...")
                    event.apply_effect()  # In a production system, you might delay effects.
                else:
                    logging.info(f"Event {event.event_id} has expired without full resolution. Forcing resolve.")
                    event.resolve()
            # Remove events that have been resolved and whose duration has expired.
            if event.resolved and (datetime.now() >= event.start_time + event.duration):
                logging.info(f"Removing event {event.event_id} from schedule.")
                self.events.remove(event)

    def trigger_random_event(self):
        """
        Randomly creates and schedules an event from a weighted selection of event types.
        """
        event_choice = random.choices(
            population=["Economic", "Diplomatic", "Environmental", "Military"],
            weights=[30, 25, 20, 25],  # Adjust weights as desired.
            k=1
        )[0]
        if event_choice == "Economic":
            event = EconomicEvent("Global market fluctuation", duration_minutes=random.randint(5, 15), economic_impact=random.uniform(0.8, 1.2))
        elif event_choice == "Diplomatic":
            event = DiplomaticEvent("Sudden diplomatic incident", duration_minutes=random.randint(5, 10), trust_delta=random.uniform(-10, 5), hostility_delta=random.uniform(5, 15))
        elif event_choice == "Environmental":
            event = EnvironmentalEvent("Severe drought affecting agriculture", duration_minutes=random.randint(10, 20), damage_factor=random.uniform(0.3, 0.7))
        elif event_choice == "Military":
            event = MilitaryEvent("Localized border skirmish", duration_minutes=random.randint(3, 8), combat_modifier=random.uniform(0.9, 1.1))
        else:
            event = None
        if event:
            self.schedule_event(event)

    def run_simulation_cycle(self, cycle_duration_seconds: int = 60):
        """
        Runs a simulation cycle: triggers random events periodically and updates the event list.
        """
        logging.info("Starting WorldEventManager simulation cycle.")
        cycle_end = datetime.now() + timedelta(seconds=cycle_duration_seconds)
        while datetime.now() < cycle_end:
            # With a probability, trigger a new random event.
            if random.random() < 0.3:  # 30% chance each cycle iteration.
                self.trigger_random_event()
            # Update events.
            self.update_events()
            # Sleep briefly to simulate time progression.
            time.sleep(2)
        logging.info("WorldEventManager simulation cycle complete.")

# -------------------------------------------------------------------------
# Standalone Testing Block
# -------------------------------------------------------------------------
if __name__ == '__main__':
    logging.info("Starting standalone simulation for events.py")
    event_manager = WorldEventManager()
    
    # Schedule some predefined events.
    event_manager.schedule_event(EconomicEvent("Recession hits major economies", 10, economic_impact=0.85))
    event_manager.schedule_event(DiplomaticEvent("Unexpected summit leads to improved relations", 8, trust_delta=+8, hostility_delta=-5))
    event_manager.schedule_event(EnvironmentalEvent("Massive wildfire devastates regions", 12, damage_factor=0.6))
    event_manager.schedule_event(MilitaryEvent("Naval confrontation in disputed waters", 7, combat_modifier=1.05))
    
    # Run a simulation cycle for 30 seconds.
    event_manager.run_simulation_cycle(cycle_duration_seconds=30)
    
    # Final update to process any lingering events.
    event_manager.update_events()
    logging.info("Events simulation complete.")
