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
    OrderBook(const std::string& symbol);
    ~OrderBook() = default;

    bool addOrder(std::shared_ptr<Order> order);
    bool removeOrder(uint64_t orderId);

    void printOrders() const;
    void printBids() const;
    void printAsks() const;
    void printTopAsk() const;
    void printTopBid() const;

    std::shared_ptr<Order> getTopBid();
    std::shared_ptr<Order> getTopAsk();
    const std::string& getSymbol() const { return symbol_; }
    
    const std::map<double, std::list<std::shared_ptr<Order>>, std::greater<double>>& getBids() const { return bids_; }
    const std::map<double, std::list<std::shared_ptr<Order>>>& getAsks() const { return asks_; }
    const std::unordered_map<uint64_t, OrderIterator>& getOrderIdMap() const { return order_id_map_; }

    const std::map<double, uint64_t, std::greater<double>>& getAggregatedBids() const { return aggregated_bids_; }
    const std::map<double, uint64_t>& getAggregatedAsks() const { return aggregated_asks_; }
    void updateAggregatedQuantity(OrderSide side, double price, uint32_t quantity);

private:
    // Simbolo do book, por exemplo "AAPL", "GOOGL", etc.
    std::string symbol_;

    // A ideia é termos uma estrutura de dados que permita acesso rápido às ordens por preço e por ID
    // Cada preço vai categorizar um nível e cada nível de preço vai conter uma lista de ordens
    // Dessa forma somos capazes de organizar a ordem por prioridade de preço e também de tempo (inserção na lista)
    std::map<double, std::list<std::shared_ptr<Order>>, std::greater<double>> bids_;
    std::map<double, std::list<std::shared_ptr<Order>>> asks_;

    // Para não termos que iterar pela lista de ordens nos níveis dos preços, temos essa segunda estrutura
    // Ela serve para buscarmos o iterador de uma ordem específica usando o ID da ordem
    // O iterador aponta para uma posicao na lista de ordens no nível de preço, dessa forma podemos remover a ordem rapidamente
    std::unordered_map<uint64_t, OrderIterator> order_id_map_;

    // Mapeia um preço diretamente para a quantidade total naquele preço. Serve como um snapshot do livro
    // Dessa forma, não precisamos iterar pela lista de ordens para saber a quantidade total naquele nível de preço
    // Quando quisermos retornar um book para o MarketDataGateway, podemos usar esses mapas agregados
    std::map<double, uint64_t, std::greater<double>> aggregated_bids_;
    std::map<double, uint64_t> aggregated_asks_;
};

#endif // ORDER_BOOK_HPP