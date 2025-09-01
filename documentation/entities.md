# Data Entity Model

This document describes the structure and purpose of the main data entities used in the Order Book & Matching Engine Simulator.

## Domain Entities

### Entity: Order

The `Order` entity represents an intention or a request sent by a client to buy or sell an asset. It is a dynamic, stateful object whose lifecycle is managed by the `Matching Engine`.

#### Attributes

| Attribute | Data Type (Conceptual) | Description |
| :--- | :--- | :--- |
| `order_id` | Unique Identifier | A unique number that identifies the order within the system. |
| `client_id` | Identifier | Identifies the client/participant who sent the order. |
| `client_order_id` | Identifier | The unique identifier for the order from the client's perspective. |
| `symbol` | Text | The ticker symbol of the asset being traded (e.g., "AAPL"). |
| `side` | Enum | The side of the order: `BUY` or `SELL`. |
| `type` | Enum | The order type: `LIMIT` (with a specified price) or `MARKET`. |
| `status` | Enum | The current state of the order in its lifecycle. |
| `time_in_force` | Enum | The order's validity rule (e.g., `DAY`, `GTC`, `IOC`). |
| `capacity` | Enum | The capacity of the participant (e.g., `AGENCY`, `PRINCIPAL`). |
| `quantity` | Numeric Object | Contains the **original quantity** and the **remaining quantity** to be executed. |
| `price` | Floating-Point Number| The limit price of the order (only applicable for `LIMIT` orders). |
| `timestamp` | DateTime | The exact moment the order was accepted by the `Matching Engine`. |

#### Lifecycle / Actions

* **Creation:** An order is instantiated from a `NewOrderCommand`. Its initial state is `NEW`, and its remaining quantity is equal to its original quantity.
* **Execution:** After a match in the `Matching Engine`, the order is updated. Its `remaining quantity` decreases, and its `status` may change to `PARTIALLY_FILLED` or `FILLED`.
* **Cancellation:** From a `CancelOrderCommand`, the order's `status` is changed to `CANCELED`, and it is removed from the `Order Book`.
* **Amendment:** From an `AmendOrderCommand`, its attributes such as `quantity` or `price` can be altered. It's implemented as a Cancel/New, first we cancel the order then we create a new one with the new values.

---

### Entity: Trade

The `Trade` entity represents an immutable historical fact. It is the record of a transaction that occurred at a specific point in time as the result of two orders matching.

#### Attributes

| Attribute | Data Type (Conceptual) | Description |
| :--- | :--- | :--- |
| `trade_id` | Unique Identifier | A unique number that identifies this specific transaction. |
| `symbol` | Text | The ticker symbol of the asset that was traded. |
| `price` | Floating-Point Number| The exact price at which the trade was executed. |
| `quantity` | Number | The number of shares exchanged in this transaction. |
| `timestamp` | DateTime | The exact moment the match occurred. |
| `aggressor_order_id` | Identifier | The ID of the order that initiated the trade (the last to arrive). |
| `passive_order_id` | Identifier | The ID of the order that was already resting in the book and was matched. |

#### Lifecycle / Actions

* **Creation:** A `Trade` is created once by the `Matching Engine` at the exact moment an order match occurs.
* **Querying:** After its creation, a `Trade` is immutable. Its information can only be read/queried, never altered.

---

## Messaging Entities

These entities are the transient vehicles for communication between the system's components.

### Commands

Commands represent an intent or a request to change the state of the system. They flow from the `Inbound Gateway` to the `Matching Engine`.

| Command | Purpose | Key Attributes |
| :--- | :--- | :--- |
| `NewOrderCommand` | To request the creation of a new order. | Carries all the raw parameters required to construct a new `Order` object. |
| `CancelOrderCommand`| To request the cancellation of an existing order. | `order_id`: The ID of the order to be canceled. |
| `AmendOrderCommand` | To request the modification of an active order. | `order_id`: The ID of the order to be modified, and the new data (e.g., new quantity, new price). |

### Events

Events are immutable objects representing a fact that has already occurred in the system.

#### Transactional Events (for the `Execution Queue`)

| Event | Purpose | `ExecType` Generated |
| :--- | :--- | :--- |
| `OrderAccepted` | To notify that a new order has been accepted for processing. | `0` (New) |
| `OrderRejected` | To notify that a request was rejected (either by the Gateway or the Engine). | `8` (Rejected) |
| `TradeExecuted` | To notify that a trade was executed. Carries a snapshot of the `Trade` and the updated state of the involved orders. | `F` (Trade) |
| `OrderPendingCancel`| To notify that a cancellation request has been received and is being processed. |`6` (Pending Cancel)|
| `OrderCanceled` | To notify that an order was successfully canceled. | `4` (Canceled) |
| `OrderAmended` | To notify that an order was successfully modified. | `5` (Replaced) |

#### Market Data Events (for the `Market Data Channel`)

| Event | Purpose | Key Attributes |
| :--- | :--- | :--- |
| `BookSnapshotEvent` | To publish a complete "picture" of the order book's state at a specific moment. | `symbol`, a list of Bids (`price`, `aggregated_quantity`), and a list of Asks (`price`, `aggregated_quantity`). |
