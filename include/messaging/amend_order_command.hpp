#ifndef AMEND_ORDER_COMMAND_HPP
#define AMEND_ORDER_COMMAND_HPP

#include "command.hpp"
#include <string>
#include <cstdint> 

class AmendOrderCommand : public Command 
{
public:
    AmendOrderCommand(uint64_t order_id, uint32_t new_quantity, double new_price);

    void execute(Engine& engine) override;
    
    uint64_t getOrderId() const { return order_id_; }
    uint32_t getNewQuantity() const { return new_quantity_; }
    double getNewPrice() const { return new_price_; }

private:
    uint64_t order_id_;
    uint32_t new_quantity_;
    double new_price_;
};

#endif // AMEND_ORDER_COMMAND_HPP