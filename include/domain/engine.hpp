#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "utils/thread_safe_queue.hpp"
#include "domain/order_book.hpp"
#include "messaging/command.hpp"
#include <atomic>

class Engine 
{
    // A engine vai consumir da command queue e processar os comandos relativos, executando os metodos necessários 
    // no caso o execute() de cada comando. Entao a principio vou codar ela com essa simples funcionalidade de consumir
    // a command queue e executar os comandos, mas depois ela vai ser responsável por toda a lógica de matching,
    // validação, etc. Bem como de construir o OrderBook e manter o estado do sistema.

public:
    explicit Engine(ThreadSafeQueue<std::unique_ptr<Command>>& command_queue);

    bool initialize();
    void consumeQueue();
    void processCommands(std::unique_ptr<Command>& command);
    bool initializeOrderBooks(const std::string& symbol);
    void printOrderBooks() const;

    std::unordered_map<std::string, std::unique_ptr<OrderBook>>& getOrderBooks() { return order_books_; }

private:
    ThreadSafeQueue<std::unique_ptr<Command>>& command_queue_;
    std::unordered_map<std::string, std::unique_ptr<OrderBook>> order_books_; // Mapeia símbolos para seus respectivos OrderBooks
    std::atomic<bool> stop_consuming_;
};

#endif // ENGINE_HPP