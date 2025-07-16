#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "utils/thread_safe_queue.hpp"
#include "messaging/command.hpp"

class Engine 
{
    // A engine vai consumir da command queue e processar os comandos relativos, executando os metodos necessários 
    // no caso o execute() de cada comando. Entao a principio vou codar ela com essa simples funcionalidade de consumir
    // a command queue e executar os comandos, mas depois ela vai ser responsável por toda a lógica de matching,
    // validação, etc. Bem como de construir o OrderBook e manter o estado do sistema.

public:
    explicit Engine(ThreadSafeQueue<std::unique_ptr<Command>>& command_queue);

    void consumeQueue();
    void processCommands(std::unique_ptr<Command>& command);

private:
    ThreadSafeQueue<std::unique_ptr<Command>>& command_queue_;
};

#endif // ENGINE_HPP