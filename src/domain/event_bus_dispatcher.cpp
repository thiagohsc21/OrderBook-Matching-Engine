#include "domain/event_bus_dispatcher.hpp"
#include "messaging/events/order_accepted_event.hpp"
#include "messaging/events/trade_executed_event.hpp"
#include "messaging/events/book_snapshot_event.hpp"
#include <iostream>

EventBusDispatcher::EventBusDispatcher(ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue, MarketDataChannel& market_data_channel)
    : event_queue_(event_queue), market_data_channel_(market_data_channel)
{
}

void EventBusDispatcher::publish(std::shared_ptr<const Event> event) {
    if (!event) {
        std::cerr << "Attempted to publish a null event." << std::endl;
        return;
    }

    if (dynamic_cast<const OrderAcceptedEvent*>(event.get()) || dynamic_cast<const TradeExecutedEvent*>(event.get()))
    {
        event_queue_.push(event);
    }
    else if (dynamic_cast<const BookSnapshotEvent*>(event.get()))
    {
        market_data_channel_.update(event);
    }
    else
    {
        std::cerr << "WARNING: Event " << event->getEventName() << " does not have a defined route.\n";
    }
}
