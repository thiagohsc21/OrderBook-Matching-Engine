#ifndef BOOK_SNAPSHOT_EVENT_HPP
#define BOOK_SNAPSHOT_EVENT_HPP

#include "messaging/events/event.hpp"
#include "domain/order_book.hpp" 
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

class OrderBook;

class BookSnapshotEvent : public Event 
{
public:
    struct PriceLevel {
        double price;
        uint64_t quantity;
    };

    // Construtor que cria a "foto" a partir de um OrderBook existente.
    // Ele copia os 'depth' melhores níveis de preço de compra e venda.
    explicit BookSnapshotEvent(const OrderBook& book, size_t depth = 5)
        : symbol_(book.getSymbol())
    {
        auto bid_it = book.getAggregatedBids().begin();
        for (size_t i = 0; i < depth && bid_it != book.getAggregatedBids().end(); ++i, ++bid_it) {
            bids_.push_back({bid_it->first, bid_it->second}); 
        }
        auto ask_it = book.getAggregatedAsks().begin();
        for (size_t i = 0; i < depth && ask_it != book.getAggregatedAsks().end(); ++i, ++ask_it) {
            asks_.push_back({ask_it->first, ask_it->second});
        }
    }

    const char* getEventName() const override { return "BookSnapshotEvent"; }

    const std::string& getSymbol() const { return symbol_; }
    const std::vector<PriceLevel>& getBids() const { return bids_; }
    const std::vector<PriceLevel>& getAsks() const { return asks_; }

private:
    std::string symbol_;
    std::vector<PriceLevel> bids_;
    std::vector<PriceLevel> asks_;
};

#endif // BOOK_SNAPSHOT_EVENT_HPP