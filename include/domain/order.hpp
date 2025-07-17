#ifndef ORDER_HPP
#define ORDER_HPP

#include "types/order_params.hpp"
#include <string>
#include <chrono>
#include <cstdint>

class Order 
{
public:
	Order(uint64_t order_id, uint64_t client_id, uint64_t client_order_id,
	      const std::string& symbol,  double price, uint32_t quantity, 
		  OrderSide side, OrderType type,
	      OrderTimeInForce time_in_force, OrderCapacity capacity, 
		  const std::chrono::system_clock::time_point& received_timestamp);

    static uint64_t getNextOrderId(){ return next_order_id_++; };
	
	uint64_t getOrderId() const { return order_id_; }
	uint64_t getClientId() const { return client_id_; }
	uint64_t getClientOrderId() const { return client_order_id_; }
	double getPrice() const { return price_; }
	uint32_t getQuantity() const { return quantity_; }
	uint32_t getFilledQuantity() const { return filled_quantity_; }
	uint32_t getRemainingQuantity() const { return remaining_quantity_; }
	const std::string& getSymbol() const { return symbol_; }
	OrderSide getSide() const { return side_; }
	OrderType getType() const { return type_; }
	OrderStatus getStatus() const { return status_; }
	OrderTimeInForce getTimeInForce() const { return time_in_force_; }
	OrderCapacity getCapacity() const { return capacity_; }
	const std::chrono::system_clock::time_point& getReceivedTimestamp() const { return received_timestamp_; }

	// outros metodos voltados para manipulacao dos dados da ordem
	// vao ser colocados com base na necessidade do sistema, que serao	
	// definidas no futuro, porque agora esta dificil de visualizar
	
private:
    static uint64_t next_order_id_; 

	uint64_t order_id_;
	uint64_t client_id_;
	uint64_t client_order_id_;

	std::string symbol_;

    double price_;
    uint32_t quantity_;
    uint32_t filled_quantity_;
    uint32_t remaining_quantity_; 

    OrderSide side_;
    OrderType type_;
    OrderStatus status_;
    OrderTimeInForce time_in_force_;
    OrderCapacity capacity_;

    std::chrono::system_clock::time_point received_timestamp_;
};

#endif // ORDER_HPP