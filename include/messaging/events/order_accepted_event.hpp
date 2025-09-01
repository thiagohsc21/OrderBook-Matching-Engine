#ifndef ORDER_ACCEPTED_EVENT_HPP
#define ORDER_ACCEPTED_EVENT_HPP

#include "messaging/events/event.hpp"
#include <memory>

class Order;

class OrderAcceptedEvent : public Event 
{
public:
    explicit OrderAcceptedEvent(std::shared_ptr<const Order> order) : order_(order) {};

    virtual const char* getEventName() const override { return "OrderAcceptedEvent"; }

    std::shared_ptr<const Order> getOrder() const { return order_; }

private:
    std::shared_ptr<const Order> order_;
};

#endif // ORDER_ACCEPTED_EVENT_HPP