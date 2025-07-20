#ifndef NEW_ORDER_COMMAND_HPP
#define NEW_ORDER_COMMAND_HPP

#include "command.hpp"
#include "types/order_params.hpp"
#include <string>
#include <cstdint> 
#include <chrono>

class NewOrderCommand : public Command 
{
public:
    NewOrderCommand(uint64_t client_order_id, uint64_t client_id, const std::string& symbol, 
                    OrderSide side, OrderType type, uint32_t quantity, double price, OrderTimeInForce tif,
                    OrderCapacity capacity, const std::chrono::system_clock::time_point& received_timestamp);

    void execute(Engine& engine) override;

    uint64_t getClientOrderId() const { return client_order_id_; }
    uint64_t getClientId() const { return client_id_; }
    const std::string& getSymbol() const { return symbol_; }    
    OrderSide getSide() const { return side_; }
    OrderType getType() const { return type_; }
    uint32_t getQuantity() const { return quantity_; }
    double getPrice() const { return price_; }
    OrderTimeInForce getTimeInForce() const { return tif_; }
    OrderCapacity getCapacity() const { return capacity_; }
    const std::chrono::system_clock::time_point& getReceivedTimestamp() const { return received_timestamp_; }

private:
    uint64_t client_order_id_;
    uint64_t client_id_;
    std::string symbol_;
    OrderSide side_;
    OrderType type_;
    uint32_t quantity_;
    double price_;
    OrderTimeInForce tif_;
    OrderCapacity capacity_;
    std::chrono::system_clock::time_point received_timestamp_;
};

#endif // NEW_ORDER_COMMAND_HPP
