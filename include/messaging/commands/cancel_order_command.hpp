#ifndef CANCEL_ORDER_COMMAND_HPP
#define CANCEL_ORDER_COMMAND_HPP

#include "command.hpp"
#include <string>
#include <cstdint> 

class CancelOrderCommand : public Command 
{
public:
    explicit CancelOrderCommand(uint64_t order_id);
    
    void execute(Engine& engine) override;

    uint64_t getOrderIdToCancel() const { return order_id_to_cancel_; }

private:
    uint64_t order_id_to_cancel_;
};

#endif // CANCEL_ORDER_COMMAND_HPP