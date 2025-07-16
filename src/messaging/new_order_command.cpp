#include "messaging/new_order_command.hpp"
#include "domain/order.hpp"
#include "types/order_params.hpp"


NewOrderCommand::NewOrderCommand(uint64_t client_order_id, uint64_t client_id, const std::string& symbol, 
         OrderSide side, OrderType type, uint32_t quantity, double price, OrderTimeInForce tif, OrderCapacity capacity)
  : client_order_id_(client_order_id), 
    client_id_(client_id), 
    symbol_(symbol),
    side_(side), 
    type_(type), 
    quantity_(quantity), 
    price_(price),
    tif_(tif),
    capacity_(capacity)
{
}

void NewOrderCommand::execute(Engine& engine) 
{
    // Cria a ordem com os parâmetros fornecidos
    Order order(Order::getNextOrderId(), client_id_, client_order_id_, symbol_, price_, quantity_, side_, type_, tif_, capacity_);

    


  
}

