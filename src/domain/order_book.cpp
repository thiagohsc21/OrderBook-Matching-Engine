#include "domain/order_book.hpp"
#include "types/order_params.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

OrderBook::OrderBook(const std::string& symbol) 
    : symbol_(symbol) 
{
}

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

        price_level_list.erase(list_iterator); // O(1)

        // Se a lista para este nível de preço ficou vazia, removemos o nível de preço do mapa O(logn)
        if (price_level_list.empty()) 
        {
            asks_.erase(priceIt);
        }
    }

    // Apagar a ordem do nosso índice
    order_id_map_.erase(it_map);
    std::cout << "Ordem de compra com ID: " << orderId << " e preço: " << price << " foi removida com sucesso\n";

    return true;
}

void OrderBook::printTopAsk() const
{
    if (asks_.empty()) 
    {
        return;
    }

    auto it = asks_.begin();
    if (it->second.empty()) 
    {
        return;
    }

    const auto& order = it->second.front();
    std::cout << "Top Ask: \nPrice: " << order->getPrice() 
              << ", Quantity: " << order->getRemainingQuantity() 
              << "\n";
}

void OrderBook::printTopBid() const
{
    if (bids_.empty()) 
    {
        return;
    }

    auto it = bids_.begin();
    if (it->second.empty()) 
    {
        return;
    }

    const auto& order = it->second.front();
    std::cout << "Top Bid: \nPrice: " << order->getPrice() 
              << ", Quantity: " << order->getRemainingQuantity() 
              << "\n";
}

void OrderBook::printAsks() const
{
    if (!asks_.empty())
        std::cout << "\nAsks:\n";

    constexpr int price_width = 6;
    constexpr int qty_width = 5;
    constexpr size_t bar_max = 130;

    for (const auto& [price, orders] : asks_)
    {
        uint64_t total_qty = 0;
        for (const auto& order : orders)
            total_qty += order->getRemainingQuantity();

        std::cout << "Price: " << std::setw(price_width) << std::fixed << std::setprecision(2) << price
                  << ", Qty: " << std::setw(qty_width) << total_qty << " ";
        size_t bar_count = std::min<size_t>(total_qty, bar_max);
        for (size_t i = 0; i < bar_count; ++i) 
            std::cout << "|";
        if (total_qty > bar_max)
            std::cout << "...";
        std::cout << "\n";
    }
    std::cout << "\n";
}

void OrderBook::printBids() const
{
    if (!bids_.empty())
        std::cout << "\nBids:\n";

    constexpr int price_width = 6;
    constexpr int qty_width = 5;
    constexpr size_t bar_max = 130;

    for (const auto& [price, orders] : bids_)
    {
        uint64_t total_qty = 0;
        for (const auto& order : orders)
            total_qty += order->getRemainingQuantity();

        std::cout << "Price: " << std::setw(price_width) << std::fixed << std::setprecision(2) << price
                  << ", Qty: " << std::setw(qty_width) << total_qty << " ";
        size_t bar_count = std::min<size_t>(total_qty, bar_max);
        for (size_t i = 0; i < bar_count; ++i) 
            std::cout << "|";
        if (total_qty > bar_max)
            std::cout << "...";
        std::cout << "\n";
    }
    std::cout << "\n";
}

void OrderBook::printOrders() const
{
    // Constantes para controle do layout
    const int LARGURA_COLUNA_PRECO = 18;
    const int LARGURA_COLUNA_QTY = 12;
    const int LARGURA_COLUNA_BARRA = 70;

    // Cabeçalho
    std::cout << std::string(LARGURA_COLUNA_PRECO + LARGURA_COLUNA_QTY + LARGURA_COLUNA_BARRA - 4, ' ') << "  [" << symbol_ << "]\n";
    std::cout << std::string(LARGURA_COLUNA_PRECO + LARGURA_COLUNA_QTY + LARGURA_COLUNA_BARRA - 4, ' ') << "Bids"
              << " | "
              << "Asks" << "\n";
    const int LARGURA_TOTAL = (LARGURA_COLUNA_PRECO + LARGURA_COLUNA_QTY + LARGURA_COLUNA_BARRA) * 2 + 3;
    std::cout << std::string(LARGURA_TOTAL, '-') << "\n";

    auto bid_it = bids_.begin();
    auto ask_it = asks_.begin();

    // Itera enquanto houver ordens em qualquer um dos lados
    while (bid_it != bids_.end() || ask_it != asks_.end())
    {
        // --- Processa a linha do lado BID (Compra) ---
        if (bid_it != bids_.end())
        {
            uint64_t total_qty = 0;
            for (const auto& order : bid_it->second)
                total_qty += order->getRemainingQuantity();
            
            size_t bar_count = std::min<size_t>(total_qty, LARGURA_COLUNA_BARRA);
            std::string bar;
            if (total_qty > LARGURA_COLUNA_BARRA) bar = std::string("...") + std::string(bar_count - 3, '|');
            else  bar = std::string(bar_count, '|');

            std::cout << std::right << std::setw(LARGURA_COLUNA_BARRA - bar.length()) << "" << bar; // Padding para a barra

            // Formatando o texto com largura fixa para Qty e Price
            std::cout << " Qty: " << std::left << std::setw(LARGURA_COLUNA_QTY - 6) << total_qty
                      << "Price: " << std::right << std::setw(LARGURA_COLUNA_PRECO - 7) << std::fixed << std::setprecision(2) << bid_it->first;

            ++bid_it;
        }
        else
        {
            // Se não há mais bids, preenche com espaços
            std::cout << std::string(LARGURA_COLUNA_BARRA + LARGURA_COLUNA_QTY + LARGURA_COLUNA_PRECO, ' ');
        }

        std::cout << " | ";

        // --- Processa a linha do lado ASK (Venda) ---
        if (ask_it != asks_.end())
        {
            uint64_t total_qty = 0;
            for (const auto& order : ask_it->second)
                total_qty += order->getRemainingQuantity();
            
            size_t bar_count = std::min<size_t>(total_qty, LARGURA_COLUNA_BARRA);
            std::string bar;
            if (total_qty > LARGURA_COLUNA_BARRA) bar = std::string(bar_count - 3, '|') + "...";
            else  bar = std::string(bar_count, '|');

            // Formatando o texto com largura fixa para Qty e Price
            std::cout << std::left 
                      << "Price: " << std::setw(LARGURA_COLUNA_PRECO - 7) << std::fixed << std::setprecision(2) << ask_it->first
                      << "Qty: " << std::setw(LARGURA_COLUNA_QTY - 5) << total_qty
                      << " " << bar;

            ++ask_it;
        }
        
        std::cout << "\n";
    }
    std::cout << std::string(LARGURA_TOTAL, '-') << std::endl;
}

std::shared_ptr<Order> OrderBook::getTopBid() 
{
    if (bids_.empty()) 
    {
        std::cerr << "No bids available.\n";
        return nullptr;
    }
    
    std::map<double, std::list<std::shared_ptr<Order>>, std::greater<double>>::const_iterator itBids = bids_.begin();
    if (itBids->second.empty()) 
    {
        std::cerr << "No orders at the top bid price.\n";
        return nullptr;
    }
    
    // Retorna o ponteiro compartilhado pra primeira ordem do nível de preço mais alto
    return itBids->second.front(); 
}

std::shared_ptr<Order> OrderBook::getTopAsk() 
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

    // Retorna o ponteiro compartilhado pra primeira ordem do nível de preço mais baixo
    return itAsks->second.front(); 
}