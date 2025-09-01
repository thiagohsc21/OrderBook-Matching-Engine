#ifndef ORDER_ACCEPTED_EVENT_HPP
#define ORDER_ACCEPTED_EVENT_HPP

#include "messaging/events/event.hpp"
#include "domain/order.hpp" 
#include <memory>
#include <string>

class Order;

class OrderAcceptedEvent : public Event 
{
public:
    explicit OrderAcceptedEvent(const Order& order) :
            order_id_(order.getOrderId()),
            client_id_(order.getClientId()),
            client_order_id_(order.getClientOrderId()),
            symbol_(order.getSymbol()),
            quantity_(order.getQuantity()),
            price_(order.getPrice()),
            side_(order.getSide())
    {}

    virtual const char* getEventName() const override { return "OrderAcceptedEvent"; }

    uint64_t getOrderId() const { return order_id_; }
    uint64_t getClientId() const { return client_id_; }
    uint64_t getClientOrderId() const { return client_order_id_; }
    const std::string& getSymbol() const { return symbol_; }
    uint32_t getQuantity() const { return quantity_; }
    double getPrice() const { return price_; }
    OrderSide getSide() const { return side_; }

private:
    const uint64_t order_id_;
    const uint64_t client_id_;
    const uint64_t client_order_id_;
    const std::string symbol_;
    const uint32_t quantity_;
    const double price_;
    const OrderSide side_;
};

#endif // ORDER_ACCEPTED_EVENT_HPP