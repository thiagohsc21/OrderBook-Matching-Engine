#include "domain/order_book.hpp"
#include "types/order_params.hpp"
#include <iostream>

bool OrderBook::addOrder(std::shared_ptr<Order> order)
{
    double price = order->getPrice();
    OrderSide side = order->getSide();

    if (side == OrderSide::Buy) 
    {
        bids_[price].push_back(order);
        order_id_map_[order->getOrderId()] = --bids_[price].end(); 
    } 
    else 
    {
        asks_[price].push_back(order);
        order_id_map_[order->getOrderId()] = --asks_[price].end(); 
    }

    std::cout << "Ordem adicionada: ID = " << order->getOrderId() 
              << ", Preço = " << price 
              << ", Quantidade = " << order->getQuantity() 
              << ", Lado = " << (side == OrderSide::Buy ? "Compra" : "Venda") 
              << "\n";
    
    return true;
}

bool OrderBook::removeOrder(uint64_t orderId) 
{
    // Encontrar a ordem no nosso índice por ID O(1)
    std::unordered_map<uint64_t, OrderIterator>::iterator it_map = order_id_map_.find(orderId);
    if (it_map == order_id_map_.end()) 
    {
        std::cerr << "Erro: Ordem " << orderId << " não encontrada para remoção.\n";
        return false;
    }

    // Obter o iterador que aponta para a ordem na lista de preços
    OrderIterator list_iterator = it_map->second;
    const std::shared_ptr<Order>& order_ptr = *list_iterator;

    double price = order_ptr->getPrice();
    OrderSide side = order_ptr->getSide();

    // Apagar a ordem do livro de Bids ou Asks
    if (side == OrderSide::Buy) 
    {
        std::map<double, std::list<std::shared_ptr<Order>>, std::greater<double>>::iterator priceIt = bids_.find(price);
        if(priceIt == bids_.end()) 
        {
            std::cerr << "Erro: Ordem " << orderId << " não encontrada.\n";
            return false;
        }

        std::list<std::shared_ptr<Order>>& price_level_list = priceIt->second;
        if (price_level_list.empty()) 
        {
            std::cerr << "Erro: Lista de preços vazia para a ordem " << orderId << ".\n";
            return false;
        }

        std::cout << "Removendo ordem de compra com ID: " << orderId << " e preço: " << price << "\n";

        price_level_list.erase(list_iterator); // O(1)
        
        // Se a lista para este nível de preço ficou vazia, removemos o nível de preço do mapa
        if (price_level_list.empty()) 
        {
            bids_.erase(priceIt); // O(logn)
        }
    } 
    else 
    { 
        std::map<double, std::list<std::shared_ptr<Order>>>::iterator priceIt = asks_.find(price);
        if(priceIt == asks_.end()) 
        {
            std::cerr << "Erro: Ordem " << orderId << " não encontrada.\n";
            return false;
        }

        std::list<std::shared_ptr<Order>>& price_level_list = priceIt->second;
        if (price_level_list.empty()) 
        {
            std::cerr << "Erro: Lista de preços vazia para a ordem " << orderId << ".\n";
            return false;
        }

        std::cout << "Removendo ordem de venda com ID: " << orderId << " e preço: " << price << "\n";

        price_level_list.erase(list_iterator); // O(1)

        // Se a lista para este nível de preço ficou vazia, removemos o nível de preço do mapa O(logn)
        if (price_level_list.empty()) 
        {
            asks_.erase(priceIt);
        }
    }

    // Apagar a ordem do nosso índice
    order_id_map_.erase(it_map);
    std::cout << "Ordem " << orderId << " removida com sucesso.\n";

    return true;
}

void OrderBook::printAsks() const
{
    std::cout << "Asks:\n";
    for (const auto& [price, orders] : asks_) 
    {
        std::cout << "Price: " << price << ", Orders: " << orders.size() << " ";
        size_t bar_count = std::min<size_t>(orders.size(), 100);
        for (size_t i = 0; i < bar_count; ++i) std::cout << "|";
        if (orders.size() > 50)
            std::cout << "...";
        std::cout << "\n";
    }
}

void OrderBook::printBids() const
{
    std::cout << "Bids:\n";
    for (const auto& [price, orders] : bids_) 
    {
        std::cout << "Price: " << price << ", Orders: " << orders.size() << " ";
        size_t bar_count = std::min<size_t>(orders.size(), 100); 
        for (size_t i = 0; i < bar_count; ++i)
            std::cout << "|";
        if (orders.size() > 50)
            std::cout << "...";
        std::cout << "\n";
    }
}

void OrderBook::printOrders() const
{
    std::cout << "Orders:\n";
    for (const auto& [price, orders] : bids_) 
    {
        std::cout << "Bid Price: " << price << ", Orders: " << orders.size() << "\n";
        for (const auto& order : orders) 
        {
            std::cout << "  Order ID: " << order->getOrderId() 
                      << ", Quantity: " << order->getQuantity() 
                      << ", Side: Buy\n";
        }
    }

    for (const auto& [price, orders] : asks_) 
    {
        std::cout << "Ask Price: " << price << ", Orders: " << orders.size() << "\n";
        for (const auto& order : orders) 
        {
            std::cout << "  Order ID: " << order->getOrderId() 
                      << ", Quantity: " << order->getQuantity() 
                      << ", Side: Sell\n";
        }
    }
}

std::shared_ptr<Order> OrderBook::getTopBid() const
{
    if (bids_.empty()) 
    {
        std::cerr << "No bids available.\n";
        return nullptr;
    }
    
    auto itBids = bids_.begin();
    if (itBids->second.empty()) 
    {
        std::cerr << "No orders at the top bid price.\n";
        return nullptr;
    }
    
    return itBids->second.front(); // Retorna a primeira ordem do nível de preço mais alto
}

std::shared_ptr<Order> OrderBook::getTopAsk() const
{
    if (asks_.empty()) 
    {
        std::cerr << "No asks available.\n";
        return nullptr;
    }
    
    auto itAsks = asks_.begin();
    if (itAsks->second.empty()) 
    {
        std::cerr << "No orders at the top ask price.\n";
        return nullptr;
    }
    
    return itAsks->second.front(); // Retorna a primeira ordem do nível de preço mais baixo
}