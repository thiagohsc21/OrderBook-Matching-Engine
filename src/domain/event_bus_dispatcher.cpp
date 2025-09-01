#include "domain/event_bus_dispatcher.hpp"
#include "messaging/events/order_accepted_event.hpp"
#include "messaging/events/trade_executed_event.hpp"
#include <iostream>

EventBusDispatcher::EventBusDispatcher(ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue)
    : event_queue_(event_queue)
{
}

void EventBusDispatcher::publish(std::shared_ptr<Event> event) {
    if (!event) {
        std::cerr << "Attempted to publish a null event." << std::endl;
        return;
    }

    if (dynamic_cast<OrderAcceptedEvent*>(event.get()) || dynamic_cast<TradeExecutedEvent*>(event.get()))
    {
        event_queue_.push(event);
    }
    else
    {
        std::cerr << "AVISO: Evento do tipo " << event->getEventName() << " não possui uma rota definida.\n";
    }
}
