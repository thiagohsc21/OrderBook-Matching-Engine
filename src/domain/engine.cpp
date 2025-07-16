#include "domain/engine.hpp"
#include <iostream>

Engine::Engine(ThreadSafeQueue<std::unique_ptr<Command>>& command_queue)
    : command_queue_(command_queue) 
{
}

void Engine::consumeQueue() 
{
    while (true) 
    {
        std::unique_ptr<Command> command;
        // Wait for a command to be available in the queue
        // This will block until a command is pushed into the queue
        command_queue_.wait_and_pop(command); 
        if (command) 
        {   
            std::cout << "Processing command...\n";
            processCommands(command);
        }
        else 
        {
            std::cerr << "Received null command.\n";
        }
    }
}

void Engine::processCommands(std::unique_ptr<Command>& command) 
{
    command->execute(*this); // Chama o execute do comando, passando o engine
}