#ifndef EVENT_BUS_DISPATCHER_HPP
#define EVENT_BUS_DISPATCHER_HPP

#include "messaging/events/event.hpp"
#include "utils/thread_safe_queue.hpp"
#include <memory>
#include <unordered_map>
#include <functional>

class EventBusDispatcher
{
public:
    EventBusDispatcher(ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue);

    // Publish an event to the event bus
    void publish(std::shared_ptr<Event> event);

private:
    ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue_;
};

#endif // EVENT_BUS_DISPATCHER_HPP