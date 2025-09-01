# System Requirements

This document outlines the functional and non-functional requirements for the Order Book & Matching Engine Simulator. The architecture reflects the final design, incorporating an Event Bus and dedicated communication channels.

## Functional Requirements (FR)

### FR01: Receive and Reject Orders

The system must have an entry point to receive and process requests (simulating the FIX protocol). It must also validate these requests and reject those that do not meet basic business criteria.

The **Inbound Gateway** is the component responsible for this function.

* **Validation:** Upon receiving a message, the Gateway performs immediate checks (e.g., valid symbol, positive price/quantity, etc.).
* **Acceptance:** If the message is valid, the Gateway translates it into the appropriate internal **Command** object (e.g., `NewOrderCommand`, `CancelOrderCommand`) and places it on the **Command Queue**.
* **Rejection:** If the message is invalid, the Gateway generates a **Rejection Event** (e.g., `OrderRejectedEvent`) and publishes it to the **Event Bus**, which then routes it to the **Event Queue**.

### FR02: Store Orders in the Order Book by Asset

Orders that are accepted but not immediately executed must be stored in an organized "Order Book," which is specific to each asset.

* The **Matching Engine** owns the collection of order books.
* A primary data structure maps a `Symbol` (e.g., "AAPL") to its respective `OrderBook` instance.
* Each `OrderBook` is composed of two sides:
    * **Bids:** Where buy orders are stored, sorted by Price-Time Priority (highest price first).
    * **Asks:** Where sell orders are stored, sorted by Price-Time Priority (lowest price first).

### FR03: Execute Orders Based on Price-Time Priority

This is the central business logic. The engine must attempt to match incoming aggressive orders against passive orders resting on the opposite side of the book.

* The matching logic follows strict **Price-Time Priority**.
* The trade price is always determined by the price of the passive order (the order that was already in the book).
* The executed quantity is the minimum of the aggressive and passive orders' available quantities.

### FR04: Dispatch Detailed Execution Events

After any significant business action, the system must generate a clear, immutable event to report what happened.

* The **Matching Engine** is responsible for generating all business events.
* These events are published to a central **Event Bus / Dispatcher**.
* The `Dispatcher` then routes each event to the appropriate channel based on its type:
    * Transactional events (`OrderAccepted`, `TradeExecuted`, `OrderCanceled`) are sent to the **Event Queue**.
    * Market data events (`BookSnapshotEvent`) are sent to the **Market Data Channel**.

### FR05: Store a Journal of Transactional Events

The system must maintain a chronological and immutable record of all transactional events. This journal is the definitive audit trail.

* The **Auditor** component is responsible for this task.
* It consumes events from the **Event Queue**.
* For each event, it formats a detailed log entry and persists it to a file (`auditor.log`).
* The format is standardized, e.g.: `[Timestamp] [Event_Type] [Key=Value Details]`

### FR06: Store a Log of All Inbound Requests (Write-Ahead Log)

The system must maintain a durable record of all raw commands received from clients to ensure recoverability.

* The **Inbound Gateway** is responsible for this task.
* Every time a message is received, it must **first** be written to the `wah_input.log` file **before** the corresponding `Command` is created and placed on the `Command Queue`.
* This log is used to reconstruct the state of the `Order Book` in case of a system failure (Recovery by Replay).

### FR07: Allow Order Cancellation and Amendment

Clients must be able to request the cancellation or modification of an active order.

* The **Inbound Gateway** receives a request (e.g., FIX `35=F` for Cancel, `35=G` for Amend) and creates the appropriate `CancelOrderCommand` or `AmendOrderCommand`.
* The command is placed on the **Command Queue**.
* The **Matching Engine** processes the command by locating the order in the `OrderBook`.
* Based on the outcome, the Engine publishes the corresponding event (e.g., `OrderCanceled`, `CancelRejected`) to the **Event Bus**.

### FR08: Publication of Market Data

The system must publish information about the state of the order book.

* This includes **Top of Book (ToB)** and **Market Depth**.
* **Architectural Flow:** After any change to an `OrderBook`, the `Matching Engine` generates a market data event (e.g., `BookSnapshotEvent`). This event is published to the **Event Bus**, which routes it to the dedicated **Market Data Channel** for consumption by the `Market Data Gateway`.

### FR09: Support for Market Orders

The system must accept Market Orders, which execute immediately against the best available prices.

* Market Orders are always aggressive and sweep the book, consuming liquidity from one or more price levels until their quantity is filled.
* The logic for this resides within the **Matching Engine**.

## Non-Functional Requirements (NFRs)

### NFR01: Low Latency
* The 99th percentile (p99) latency for order processing must be **less than 10 milliseconds (ms)**.

### NFR02: High Throughput
* The system must support a sustained throughput of at least **1,000 orders per second**.

### NFR03: Concurrency
* The system must safely handle simultaneous requests from multiple threads without data corruption, race conditions, or deadlocks.

### NFR04: Resilience and Correctness
* Order/Trade Loss Rate: **0%**.
* Rejection Rate due to internal system failure: **0%**.

### NFR05: Observability
* The `Auditor`'s journal must log 100% of all transactional events.
* The delay between an event occurring and being persisted in the journal must be **less than 1 second**.
