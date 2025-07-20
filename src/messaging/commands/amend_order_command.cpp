#include "messaging/commands/amend_order_command.hpp"

AmendOrderCommand::AmendOrderCommand(uint64_t order_id, uint32_t new_quantity, double new_price)
    : order_id_(order_id), 
      new_quantity_(new_quantity), 
      new_price_(new_price) 
{
}

void AmendOrderCommand::execute(Engine& engine) 
{
    // Implementation of the execute method, which will interact with the MatchingEngine
    // to process the amend order command.
    // This is where the logic for modifying an existing order in the order book would go.
}