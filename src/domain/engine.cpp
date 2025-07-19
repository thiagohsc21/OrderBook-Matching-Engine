#include "domain/engine.hpp"
#include <iostream>
#include "messaging/new_order_command.hpp"
#include "messaging/command.hpp"

Engine::Engine(ThreadSafeQueue<std::unique_ptr<Command>>& command_queue)
    : command_queue_(command_queue),
      stop_consuming_(false)
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

    // Coisas que devo fazer antes de adicionar a ordem:
    // 1. Verificar se a ordem é agressiva (se é uma compra e o TopAsk <= ao preço da ordem, ou se é uma venda e o TopBid >= ao preço da ordem)
    // 2. Se for agressiva, fazer o matching com as ordens existentes e depois adicionar a ordem na fila de ordens
    // 3. Se não for agressiva, adicionar a ordem na fila de ordens
    // Passamos o shared_ptr<Order> e também o OrderBook para a função de matching

    //imprime parametros da ordem
    std::cout << "Processing new order with ID: " << new_order_ptr->getOrderId() 
              << ", Symbol: " << symbol 
              << ", Side: " << (new_order_ptr->getSide() == OrderSide::Buy ? "Buy" : "Sell") 
              << ", Price: " << new_order_ptr->getPrice() 
              << ", Quantity: " << new_order_ptr->getQuantity() 
              << "\n";

    orderBookPtr->printOrders();
   
    tryMatchOrderWithTopOfBook(new_order_ptr, *orderBookPtr);

    if (new_order_ptr->getStatus() != OrderStatus::Filled) 
    {
        return orderBookPtr->addOrder(new_order_ptr);
    }

    return true;
}

void Engine::tryMatchOrderWithTopOfBook(std::shared_ptr<Order> agressive_order, OrderBook& orderBook) 
{
    bool is_buy_side = agressive_order->getSide() == OrderSide::Buy;
    std::shared_ptr<Order> passive_order = is_buy_side ? orderBook.getTopAsk() : orderBook.getTopBid();
    bool is_agressive = (is_buy_side && passive_order && agressive_order->getPrice() >= passive_order->getPrice()) ||
                        (!is_buy_side && passive_order && agressive_order->getPrice() <= passive_order->getPrice());

    if (passive_order && is_agressive)
    {   
        while(passive_order && agressive_order->getRemainingQuantity() > 0 && is_agressive) 
        {
            // Atualiza a ordem agressiva com as novas quantidades
            uint32_t filled_qty = std::min<uint32_t>(agressive_order->getRemainingQuantity(), passive_order->getRemainingQuantity());
            uint32_t remaining_quantity = agressive_order->getRemainingQuantity() - filled_qty;
            double averagePrice = passive_order->getPrice() * filled_qty;

            agressive_order->setRemainingQuantity(remaining_quantity);
            agressive_order->setFilledQuantity(agressive_order->getFilledQuantity() + filled_qty);
            agressive_order->setFilledPrice(agressive_order->getFilledPrice() + averagePrice);

            std::cout << "[" << agressive_order->getSymbol() << "] Matched Order ID: " << agressive_order->getOrderId() << " with Order ID: " << passive_order->getOrderId() << ", Filled Quantity: " << filled_qty << ", Remaining Quantity: " << agressive_order->getRemainingQuantity() << "\n";

            // Atualiza a ordem passiva com as novas quantidades
            passive_order->setRemainingQuantity(passive_order->getRemainingQuantity() - filled_qty);
            passive_order->setFilledQuantity(passive_order->getFilledQuantity() + filled_qty);

            if (passive_order->getRemainingQuantity() == 0) 
            {   
                passive_order->setOrderStatus(OrderStatus::Filled);
                std::cout << "Order with ID: " << passive_order->getOrderId() << " is fully filled and will be removed from the book.\n";

                orderBook.removeOrder(passive_order->getOrderId());
                // dispara Event()
            }

            passive_order = is_buy_side ? orderBook.getTopAsk() : orderBook.getTopBid();
            is_agressive = (is_buy_side && passive_order && agressive_order->getPrice() >= passive_order->getPrice()) ||
                           (!is_buy_side && passive_order && agressive_order->getPrice() <= passive_order->getPrice());
        }

        if (agressive_order->getFilledQuantity() == 0) 
        {
            std::cerr << "Order with ID: " << agressive_order->getOrderId() << " was not filled after matching parameters.\n";
            return;
        }

        double finalAveragePrice = agressive_order->getFilledPrice() / agressive_order->getFilledQuantity();
        if (agressive_order->getRemainingQuantity() == 0) 
        {
            agressive_order->setOrderStatus(OrderStatus::Filled);
            std::cout << "Order with ID: " << agressive_order->getOrderId() << " is fully filled with average price: " << finalAveragePrice << "\n";
        } 
        else 
        {
            agressive_order->setOrderStatus(OrderStatus::PartiallyFilled);
            std::cout << "Order with ID: " << agressive_order->getOrderId() << " is partially filled with average price: " << finalAveragePrice << "\n";
        }
    } 
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
        // orderBookPtr->printBids();
        // orderBookPtr->printAsks();
        // orderBookPtr->printTopAsk();
        // orderBookPtr->printTopBid();
    }
}