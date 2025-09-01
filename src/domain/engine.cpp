#include "domain/engine.hpp"
#include "domain/trade.hpp"
#include <iostream>
#include <sstream>
#include "messaging/commands/new_order_command.hpp"
#include "messaging/commands/command.hpp"
#include "messaging/events/trade_executed_event.hpp"
#include "messaging/events/order_accepted_event.hpp"
#include "utils/timestamp_formatter.hpp" 

Engine::Engine(ThreadSafeQueue<std::unique_ptr<Command>>& command_queue)
    : command_queue_(command_queue)
{
}

bool Engine::initialize()
{
    std::vector<std::string> symbols = {"GOOG", "AMZN", "AAPL", "MSFT"};
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

void Engine::run() 
{
    while (true) 
    {
        std::unique_ptr<Command> command;
        
        // wait_and_pop bloqueia até que haja um item OU a fila seja desligada
        // Se wait_and_pop retornar false, significa que a fila foi desligada e está vazia, thread deve terminar
        if (!command_queue_.wait_and_pop(command)) {
            break; 
        }
        
        // Se chegamos aqui, temos um comando válido e devemos executa-lo
        command->execute(*this);
    }
    std::cout << "Engine has finished consuming." << std::endl;
}

void Engine::publishEvent(std::shared_ptr<const Event> event)
{
    // publica o evento
}

bool Engine::processNewOrderCommand(std::shared_ptr<Order> new_order_ptr)
{   
    // order_ptr: imprime o ponteiro compartilhado (std::shared_ptr<Order>), mostra o endereço do objeto gerenciado.
    // &(*order_ptr): imprime o endereço do objeto Order apontado pelo ponteiro (desreferencia e pega o endereço real do objeto).
    // &order_ptr: imprime o endereço da variável local order_ptr (onde o ponteiro está armazenado na stack).
    // std::cout << new_order_ptr << " " << &(*new_order_ptr) << " " << &new_order_ptr << std::endl;
    
    if (!new_order_ptr) 
    {
        std::cerr << "Received null order pointer in processNewOrderCommand.\n";
        return false; 
    }

    const std::string& symbol = new_order_ptr->getSymbol();
    std::unordered_map<std::string, std::unique_ptr<OrderBook>>::iterator it = order_books_.find(symbol);
    
    if (it == order_books_.end()) 
    {
        std::cerr << "OrderBook for symbol " << symbol << " not found when processing new order command.\n";
        return false; 
    }

    // O ponteiro inteligente só gerencia a memória pra gente, mas isso seria equivalente a e it->second é OrderBook*, então (OrderBook*)& orderBookPtr = it->second; 
    // orderBookPtr é um end de memória que aponta pra outro end que é o objeto OrderBook real. A diferença do ponteiro inteligente é que não usamos * explicitamente
    std::unique_ptr<OrderBook>& orderBookPtr = it->second;
    if (!orderBookPtr) 
    {
        std::cerr << "OrderBook for symbol " << symbol << " is null when processing new order command.\n";
        return false; 
    }

    std::cout << "Processing new order with ID: " << new_order_ptr->getOrderId() << ", Symbol: " << symbol << ", Side: " << (new_order_ptr->getSide() == OrderSide::Buy ? "Buy" : "Sell") << ", Price: " << new_order_ptr->getPrice() << ", Quantity: " << new_order_ptr->getQuantity() << "\n";
    orderBookPtr->printOrders();
   
    tryMatchOrderWithTopOfBook(new_order_ptr, *orderBookPtr);

    if (!new_order_ptr->isFilled() && orderBookPtr->addOrder(new_order_ptr)) 
    {
        std::shared_ptr<OrderAcceptedEvent> oder_accepted_event = std::make_shared<OrderAcceptedEvent>(new_order_ptr);
        publishEvent(oder_accepted_event);
    }

    return true;
}

void Engine::tryMatchOrderWithTopOfBook(std::shared_ptr<Order> aggressive_order, OrderBook& orderBook) 
{
    bool is_buy_side = aggressive_order->getSide() == OrderSide::Buy;
    std::shared_ptr<Order> passive_order = is_buy_side ? orderBook.getTopAsk() : orderBook.getTopBid();
    bool is_aggresive = (is_buy_side && passive_order && aggressive_order->getPrice() >= passive_order->getPrice()) ||
                        (!is_buy_side && passive_order && aggressive_order->getPrice() <= passive_order->getPrice());

    if (passive_order && is_aggresive)
    {   
        while(passive_order && aggressive_order->getRemainingQuantity() > 0 && is_aggresive) 
        {
            // Atualiza a ordem agressiva com as novas quantidades
            uint32_t filled_qty = std::min<uint32_t>(aggressive_order->getRemainingQuantity(), passive_order->getRemainingQuantity());

            aggressive_order->applyFill(filled_qty, passive_order->getPrice());
            passive_order->applyFill(filled_qty, passive_order->getPrice());

            std::shared_ptr<Trade> trade = std::make_shared<Trade>(
                Trade::getNextTradeId(), aggressive_order->getOrderId(), passive_order->getOrderId(),
                orderBook.getSymbol(), passive_order->getPrice(), filled_qty, std::chrono::system_clock::now()
            );
            
            std::cout << "#TRADE <" << trade->getTradeId() << "> executed <" << trade->getSymbol() << "> - Qty: " << trade->getQuantity() << " @ Price: " << trade->getPrice()
                    << " | Aggressive ID: <" << trade->getAggressiveOrderId() << ">, Passive ID: <" << trade->getPassiveOrderId() << ">" << " | Aggressive Remaining: " << aggressive_order->getRemainingQuantity()
                    << ", Passive Remaining: " << passive_order->getRemainingQuantity() << ", Filled Qty: " << filled_qty << "\n";

            std::shared_ptr<TradeExecutedEvent> trade_event = std::make_shared<TradeExecutedEvent>(trade, aggressive_order, passive_order);
            publishEvent(trade_event);

            if (passive_order->isFilled()) 
            {
                std::cout << "Order with ID: " << passive_order->getOrderId() << " is fully filled with average price: " << passive_order->getAveragePrice() << "\n";
                orderBook.removeOrder(passive_order->getOrderId());
            }

            passive_order = is_buy_side ? orderBook.getTopAsk() : orderBook.getTopBid();
            is_aggresive = (is_buy_side && passive_order && aggressive_order->getPrice() >= passive_order->getPrice()) ||
                           (!is_buy_side && passive_order && aggressive_order->getPrice() <= passive_order->getPrice());
        }

        std::cout << "Order with ID: " << aggressive_order->getOrderId() << " is " << (aggressive_order->getRemainingQuantity() == 0 ? "fully" : "partially") << " filled with average price: " << aggressive_order->getAveragePrice() << " and will not be added to the book\n";
    } 
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
        // orderBookPtr->printBids();
        // orderBookPtr->printAsks();
        // orderBookPtr->printTopAsk();
        // orderBookPtr->printTopBid();
    }
}