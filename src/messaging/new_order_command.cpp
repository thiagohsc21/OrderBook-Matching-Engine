#include "messaging/new_order_command.hpp"
#include "domain/order.hpp"
#include "utils/timestamp_formatter.hpp"
#include "types/order_params.hpp"
#include "domain/engine.hpp"
#include "domain/order_book.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>


NewOrderCommand::NewOrderCommand(uint64_t client_order_id, uint64_t client_id, const std::string& symbol, 
         OrderSide side, OrderType type, uint32_t quantity, double price, OrderTimeInForce tif, OrderCapacity capacity,
         const std::chrono::system_clock::time_point& received_timestamp)
  : client_order_id_(client_order_id), 
    client_id_(client_id), 
    symbol_(symbol),
    side_(side), 
    type_(type), 
    quantity_(quantity), 
    price_(price),
    tif_(tif),
    capacity_(capacity),
    received_timestamp_(received_timestamp)
{
}

void NewOrderCommand::execute(Engine& engine) 
{

    std::shared_ptr<Order> order_ptr = std::make_shared<Order>(
        Order::getNextOrderId(), client_id_, client_order_id_,
        symbol_, price_, quantity_, side_, type_, tif_, capacity_, received_timestamp_
    );

    engine.processNewOrderCommand(order_ptr);
}

