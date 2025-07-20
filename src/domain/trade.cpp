#include "domain/trade.hpp"

uint64_t Trade::next_trade_id_ = 1; // Initialize static member

Trade::Trade(uint64_t trade_id, uint64_t agressive_order_id, uint64_t passive_order_id, 
             const std::string& symbol, double price, uint32_t quantity, 
             const std::chrono::system_clock::time_point& timestamp)
    : trade_id_(trade_id), 
      aggressive_order_id_(agressive_order_id),
      passive_order_id_(passive_order_id),
      symbol_(symbol),
      price_(price),
      quantity_(quantity),
      timestamp_(timestamp) 
{
}

