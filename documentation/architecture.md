## Architecture Components Description
<img width="1007" height="367" alt="image" src="https://github.com/user-attachments/assets/550f761f-dc97-47c5-a19c-3a0571e65c23" />

This documentation describes the responsibilities and interactions of each main component of the Order Book Simulator.

### 1. Inbound Gateway

* **Primary Responsibility:** To serve as the system's entry point, responsible for receiving, securely logging, and translating client requests into internal commands.
* **Inputs:** Raw text commands (simulating the FIX protocol).
* **Processing:**
    1.  **Logging (Write-Ahead Log):** Immediately writes an exact copy of the raw command to the `Input Log` for recovery purposes.
    2.  **Interpretation:** Parses the command to understand the client's intent (e.g., `NEW_ORDER`, `CANCEL_ORDER`).
    3.  **Syntactic Validation:** If the message is invalid (e.g., missing a required field, incorrect format), it generates an `OrderRejectedEvent`.
    4.  **Command Creation (Factory):** If the message is valid, it instantiates the appropriate `Command` object (e.g., `NewOrderCommand`), populating it with data from the request.
* **Outputs:** A `Command` object to the `Command Queue` (on success) or an `OrderRejectedEvent` published to the `Event Bus` (on validation failure).

### 2. Input Log

* **Primary Responsibility:** To provide a durable, chronological record of all requests that entered the system, for replay and disaster recovery purposes.
* **Inputs:** Raw text strings from the `Inbound Gateway`.
* **Processing:** Simply appends the received string to a file on disk.
* **Outputs:** The `wah_input.log` file.

### 3. Command Queue

* **Primary Responsibility:** To decouple the `Inbound Gateway` from the `Matching Engine`, acting as a safe, ordered buffer for tasks (commands) to be executed.
* **Inputs:** `Command` objects (`NewOrderCommand`, `CancelOrderCommand`, etc.).
* **Processing:** Stores commands in a thread-safe FIFO (First-In, First-Out) structure.
* **Outputs:** `Command` objects, one by one, to the `Matching Engine`.

### 4. Matching Engine

* **Primary Responsibility:** The brain of the system. It orchestrates the execution of all commands, applies business logic, and maintains the market state via the `Order Book`.
* **Inputs:** `Command` objects from the `Command Queue`.
* **Processing:**
    1.  Dequeues a `Command` and executes it polymorphically (`command->execute(engine)`).
    2.  All interaction with the `Order Book` (querying, inserting, removing orders) is performed here. This includes business-level validation.
    3.  After each significant state change, it generates one or more `Event` objects (`OrderAccepted`, `TradeExecuted`, `BookUpdateEvent`, etc.) to describe the result.
* **Outputs:** `Event` objects that are **published** to the `Event Bus / Dispatcher`.

### 5. Order Book

* **Primary Responsibility:** To maintain the current, ordered state of all active buy (Bids) and sell (Asks) orders. It is the in-memory database for the `Matching Engine`.
* **Inputs:** Requests for insertion, removal, and queries from the `Matching Engine`.
* **Processing:** Organizes orders according to Price-Time priority.
* **Outputs:** Responses to the `Matching Engine`'s queries.

### 6. Event Bus / Dispatcher

* **Primary Responsibility:** To act as a central event router. It completely decouples the event producers (`Matching Engine`, `Inbound Gateway`) from the various outbound channels.
* **Inputs:** All `Event` objects published by the `Matching Engine` and `Inbound Gateway`.
* **Processing:** Analyzes the **type** of each received event and forwards it to the appropriate channel.
* **Outputs:** Transactional events are pushed to the `Event Queue`. Market data events are pushed to the `Market Data Channel`.

### 7. Event Queue

* **Primary Responsibility:** To serve as a thread-safe buffer for critical transactional events that cannot be lost.
* **Inputs:** Transactional `Event` objects from the `Event Bus / Dispatcher`.
* **Processing:** Stores events in a thread-safe FIFO structure.
* **Outputs:** `Event` objects to all its consumers (the `Outbound Gateway` and the `Auditor`).

### 8. Market Data Channel

* **Primary Responsibility:** To serve as a distribution channel for high-volume market data, where only the most recent state is important.
* **Inputs:** Market data `Event` objects from the `Event Bus / Dispatcher`.
* **Processing:** Implemented with the "Shared State + Notification" pattern to ensure consumers always get the latest data.
* **Outputs:** The latest `MarketDataSnapshotEvent` to the `Market Data Gateway`.

### 9. Outbound Gateway

* **Primary Responsibility:** To communicate the results of transactions back to the end client (simulating FIX).
* **Inputs:** `Event` objects from the `Event Queue`.
* **Processing:** Translates internal event objects into `ExecutionReport` messages in the FIX format.
* **Outputs:** Response messages to the client.

### 10. Auditor (Journaler)

* **Primary Responsibility:** To create a complete, persistent, human-readable audit trail of all transactional events.
* **Inputs:** `Event` objects from the `Event Queue`.
* **Processing:** Formats each event into a standardized text line and persists it to disk.
* **Outputs:** A text file (`auditor.log`).

### 11. Market Data Gateway

* **Primary Responsibility:** To distribute real-time market data to all interested clients.
* **Inputs:** The latest `MarketDataSnapshotEvent` available on the `Market Data Channel`.
* **Processing:** Formats the snapshot data into a suitable broadcast format (e.g., JSON).
* **Outputs:** A continuous stream of market data to clients.



