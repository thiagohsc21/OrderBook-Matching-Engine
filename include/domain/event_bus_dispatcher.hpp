#ifndef EVENT_BUS_DISPATCHER_HPP
#define EVENT_BUS_DISPATCHER_HPP

#include "messaging/events/event.hpp"
#include "utils/thread_safe_queue.hpp"
#include "utils/market_data_channel.hpp"
#include <memory>
#include <unordered_map>
#include <functional>

class EventBusDispatcher
{
public:
    EventBusDispatcher(ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue, MarketDataChannel& market_data_channel);

    // Publish an event to the event bus
    void publish(std::shared_ptr<const Event> event);

private:
    ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue_;
    MarketDataChannel& market_data_channel_;
};

#endif // EVENT_BUS_DISPATCHER_HPP