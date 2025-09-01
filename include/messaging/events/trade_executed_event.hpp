// include/messaging/events/trade_executed_event.hpp

#ifndef TRADE_EXECUTED_EVENT_HPP
#define TRADE_EXECUTED_EVENT_HPP

#include "messaging/events/event.hpp"
#include "domain/trade.hpp"
#include "domain/order.hpp"
#include <memory>
#include <string>
#include <cstdint>

class TradeExecutedEvent : public Event 
{
public:
    // O construtor copia os dados do Trade e os estados atualizados das ordens.
    TradeExecutedEvent(const Trade& trade, const Order& aggressive_order, const Order& passive_order)
        : trade_id_(trade.getTradeId()),
          symbol_(trade.getSymbol()),
          price_(trade.getPrice()),
          quantity_(trade.getQuantity()),
          aggressive_order_id_(aggressive_order.getOrderId()),
          passive_order_id_(passive_order.getOrderId()),
          aggressive_order_status_(aggressive_order.getStatus()),
          passive_order_status_(passive_order.getStatus()),
          aggressive_remaining_qty_(aggressive_order.getRemainingQuantity()),
          passive_remaining_qty_(passive_order.getRemainingQuantity())
    {}

    const char* getEventName() const override { return "TradeExecutedEvent"; }

    uint64_t getTradeId() const { return trade_id_; }
    const std::string& getSymbol() const { return symbol_; }
    double getPrice() const { return price_; }
    uint32_t getQuantity() const { return quantity_; }
    uint64_t getAggressiveOrderId() const { return aggressive_order_id_; }
    uint64_t getPassiveOrderId() const { return passive_order_id_; }
    OrderStatus getAggressiveOrderStatus() const { return aggressive_order_status_; }
    OrderStatus getPassiveOrderStatus() const { return passive_order_status_; }
    uint32_t getAggressiveRemainingQuantity() const { return aggressive_remaining_qty_; }
    uint32_t getPassiveRemainingQuantity() const { return passive_remaining_qty_; }

private:
    // Dados do Trade
    const uint64_t trade_id_;
    const std::string symbol_;
    const double price_;
    const uint32_t quantity_;

    // Dados das Ordens envolvidas (no momento do evento)
    const uint64_t aggressive_order_id_;
    const uint64_t passive_order_id_;
    const OrderStatus aggressive_order_status_;
    const OrderStatus passive_order_status_;
    const uint32_t aggressive_remaining_qty_;
    const uint32_t passive_remaining_qty_;
};

#endif // TRADE_EXECUTED_EVENT_HPP