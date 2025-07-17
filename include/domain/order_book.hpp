#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include "domain/order.hpp"
#include <map>
#include <list>
#include <cstdint>
#include <memory> 
#include <unordered_map> 
#include <functional> 

using OrderIterator = std::list<std::shared_ptr<Order>>::iterator;

class OrderBook 
{
public:
    OrderBook() = default;
    ~OrderBook() = default;

    bool addOrder(std::shared_ptr<Order> order);
    bool removeOrder(uint64_t orderId);

    void printOrders() const;
    void printBids() const;
    void printAsks() const;
    void printTopAsk() const;
    void printTopBid() const;
    std::shared_ptr<Order> getTopBid() const;
    std::shared_ptr<Order> getTopAsk() const;

    const std::map<double, std::list<std::shared_ptr<Order>>, std::greater<double>>& getBids() const { return bids_; }
    const std::map<double, std::list<std::shared_ptr<Order>>>& getAsks() const { return asks_; }
    const std::unordered_map<uint64_t, OrderIterator>& getOrderIdMap() const { return order_id_map_; }

private:
    // A ideia é termos uma estrutura de dados que permita acesso rápido às ordens por preço e por ID
    // Cada preço vai categorizar um nível e cada nível de preço vai conter uma lista de ordens
    std::map<double, std::list<std::shared_ptr<Order>>, std::greater<double>> bids_;
    std::map<double, std::list<std::shared_ptr<Order>>> asks_;

    // Para não termos que iterar pela lista de ordens nos níveis dos preços, temos essa segunda estrutura
    // Ela serve para buscarmos o iterador de uma ordem específica usando o ID da ordem
    // O iterador aponta para uma posicao na lista de ordens no nível de preço, dessa forma podemos remover a ordem rapidamente
    std::unordered_map<uint64_t, OrderIterator> order_id_map_;
};

#endif // ORDER_BOOK_HPP