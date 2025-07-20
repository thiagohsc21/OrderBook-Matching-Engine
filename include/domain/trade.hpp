#ifndef TRADE_HPP
#define TRADE_HPP

#include <cstdint>
#include <string>
#include <chrono>

class Trade
{
public:
    Trade(uint64_t trade_id, uint64_t aggressive_order_id, uint64_t passive_order_id, 
          const std::string& symbol, double price, uint32_t quantity, 
          const std::chrono::system_clock::time_point& timestamp);

    static uint64_t getNextTradeId() { return next_trade_id_++; }
    static void resetTradeIdCounter() { next_trade_id_ = 1; }

    uint64_t getTradeId() const { return trade_id_; }
    uint64_t getAggressiveOrderId() const { return aggressive_order_id_; }
    uint64_t getPassiveOrderId() const { return passive_order_id_; }
    const std::string& getSymbol() const { return symbol_; }
    double getPrice() const { return price_; }
    uint32_t getQuantity() const { return quantity_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
private:
    static uint64_t next_trade_id_; 

    uint64_t trade_id_;
    uint64_t aggressive_order_id_;
    uint64_t passive_order_id_;
    std::string symbol_;
    double price_;
    uint32_t quantity_;
    std::chrono::system_clock::time_point timestamp_;
};

#endif // TRADE_HPP