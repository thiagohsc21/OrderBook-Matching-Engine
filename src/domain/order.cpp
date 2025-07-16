#include "types/order_params.hpp"
#include "domain/order.hpp"
#include <chrono>
#include <string>

uint64_t Order::next_order_id_ = 1; // Initialize static member

Order::Order(uint64_t order_id, uint64_t client_id, uint64_t client_order_id,
             const std::string& symbol, double price, uint32_t quantity, 
             OrderSide side, OrderType type,
             OrderTimeInForce time_in_force, OrderCapacity capacity)
    : order_id_(order_id), 
      client_id_(client_id),
      client_order_id_(client_order_id),
      symbol_(symbol),
      price_(price),
      quantity_(quantity),
      filled_quantity_(0),                                  
      remaining_quantity_(quantity),                        
      side_(side),
      type_(type),
      status_(OrderStatus::New),                            
      time_in_force_(time_in_force),
      capacity_(capacity),
      received_timestamp_(std::chrono::system_clock::now()) 
{
}

uint64_t Order::getNextOrderId() 
{ 
  return next_order_id_++; 
}