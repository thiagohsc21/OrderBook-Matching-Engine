#ifndef TRADE_EXECUTED_EVENT_HPP
#define TRADE_EXECUTED_EVENT_HPP

#include "messaging/events/event.hpp"
#include "domain/trade.hpp"
#include <memory>

class Order;

class TradeExecutedEvent : public Event 
{
public:
    TradeExecutedEvent(std::shared_ptr<const Trade> trade, std::shared_ptr<const Order> aggressive_order, std::shared_ptr<const Order> passive_order)
        : trade_(trade), 
          aggressive_order_(aggressive_order), 
          passive_order_(passive_order) {}
    
    std::shared_ptr<const Trade> getTrade() const { return trade_; }
    std::shared_ptr<const Order> getAggressiveOrder() const { return aggressive_order_; }
    std::shared_ptr<const Order> getPassiveOrder() const { return passive_order_; }

private:
    std::shared_ptr<const Trade> trade_;
    std::shared_ptr<const Order> aggressive_order_;
    std::shared_ptr<const Order> passive_order_;
};

#endif // TRADE_EXECUTED_EVENT_HPP