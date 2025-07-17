#include "domain/engine.hpp"
#include <iostream>

Engine::Engine(ThreadSafeQueue<std::unique_ptr<Command>>& command_queue)
    : command_queue_(command_queue),
      stop_consuming_(false)
{
}

bool Engine::initialize()
{
    std::vector<std::string> symbols = {"AAPL", "AMZN", "GOOG"};
    for (const std::string& symbol : symbols) 
    {
        if (!initializeOrderBooks(symbol)) 
        {
            std::cerr << "Failed to initialize OrderBook for symbol: " << symbol << "\n";
            return false; 
        }
    }
    return true;
}

void Engine::consumeQueue() 
{
    while (true) 
    {
        std::unique_ptr<Command> command;
        
        // wait_and_pop agora bloqueia até que haja um item OU a fila seja desligada.
        if (!command_queue_.wait_and_pop(command)) {
            // Se wait_and_pop retornar false, significa que a fila foi desligada e está vazia.
            // É o sinal para a thread terminar.
            break; 
        }
        
        // Se chegamos aqui, temos um comando válido.
        processCommands(command);
    }
    std::cout << "Engine has finished consuming." << std::endl;
}

void Engine::processCommands(std::unique_ptr<Command>& command) 
{
    command->execute(*this); // Chama o execute do comando, passando o engine
}

bool Engine::initializeOrderBooks(const std::string& symbol) 
{
    // Verifica se o OrderBook já existe para o símbolo
    if (order_books_.find(symbol) != order_books_.end()) 
    {
        std::cerr << "OrderBook for symbol " << symbol << " already exists.\n";
        return false; 
    }

    // Cria um novo OrderBook e adiciona ao mapa
    order_books_[symbol] = std::make_unique<OrderBook>(symbol);
    std::cout << "OrderBook for symbol " << symbol << " initialized successfully.\n";
    return true;
}

void Engine::printOrderBooks() const 
{
    if (order_books_.empty()) 
    {
        std::cout << "No OrderBooks available.\n";
        return;
    }

    for (const auto& [symbol, orderBookPtr] : order_books_) 
    {
        orderBookPtr->printOrders();
        orderBookPtr->printBids();
        orderBookPtr->printAsks();
        orderBookPtr->printTopAsk();
        orderBookPtr->printTopBid();
    }
}