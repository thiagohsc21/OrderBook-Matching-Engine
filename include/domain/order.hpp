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
	static void resetOrderIdCounter() { next_order_id_ = 1; }
	uint64_t getOrderId() const { return order_id_; }
	uint64_t getClientId() const { return client_id_; }
	uint64_t getClientOrderId() const { return client_order_id_; }
	const std::string& getSymbol() const { return symbol_; }
	double getPrice() const { return price_; }
	double getTotalFilledValue() const { return total_filled_value_; }
	uint32_t getQuantity() const { return quantity_; }
	uint32_t getFilledQuantity() const { return filled_quantity_; }
	uint32_t getRemainingQuantity() const { return remaining_quantity_; }
	OrderSide getSide() const { return side_; }
	OrderType getType() const { return type_; }
	OrderStatus getStatus() const { return status_; }
	OrderTimeInForce getTimeInForce() const { return time_in_force_; }
	OrderCapacity getCapacity() const { return capacity_; }
	const std::chrono::system_clock::time_point& getReceivedTimestamp() const { return received_timestamp_; }
	double getAveragePrice() const { return filled_quantity_ > 0 ? total_filled_value_ / filled_quantity_ : 0.0; }

	bool isFilled() const { return status_ == OrderStatus::Filled; }
	bool isPartiallyFilled() const { return status_ == OrderStatus::PartiallyFilled; }
	bool isNew() const { return status_ == OrderStatus::New; }

	bool applyFill(uint32_t filled_quantity, double filled_price);
	
private:
	void setFilledQuantity(uint32_t filled_quantity) { filled_quantity_ = filled_quantity; }
	void setTotalFilledValue(double total_filled_value) { total_filled_value_ = total_filled_value; }
	void setRemainingQuantity(uint32_t quantity) { remaining_quantity_ = quantity; }
	void setOrderStatus(OrderStatus status) { status_ = status; }

    static uint64_t next_order_id_; 
	uint64_t order_id_;
	uint64_t client_id_;
	uint64_t client_order_id_;
	std::string symbol_;
    double price_;
	double total_filled_value_;
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