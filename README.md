# Financial Matching Engine Simulator 

![Status](https://img.shields.io/badge/status-in%20development-yellow)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## About the Project

This project is a simulator of a financial order matching engine (Matching Engine) and its respective order book, developed in modern C++ with a strong focus on design, performance, and concurrency. The goal is to build a solid foundation that simulates the core of a stock exchange, following software engineering best practices.

## Architecture Overview

The system is modeled as an event-processing pipeline. External requests (commands) are received by an `Inbound Gateway`, translated into an internal format, and placed into a command queue. A dedicated `Matching Engine` consumes these commands, applies business logic against the `Order Book`, and generates result events.

These events are published to a central `Event Bus`, which acts as a router, directing transactional and market data events to distinct, specialized output channels. This ensures that critical execution notifications are not delayed by high-volume market data updates.

## Full Project Documentation

The entire planning and design phase has been documented in detail. Refer to the following documents:

* **[System Requirements](documentation/requirements.md):** Defines *what* the system must do (Functional Requirements) and *how well* it must operate (Non-Functional Requirements and Service Level Objective (SLOs)).
* **[System Architecture](documentation/architecture.md):** Describes the *how*, including the high-level design of components, the complete flow diagram, and the responsibilities of each architectural part.
* **[Data Model and Entities](documentation/entities.md):** Details the *building blocks* of the system, specifying each domain entity (`Order`, `Trade`) and messaging entity (`Command`, `Event`).

