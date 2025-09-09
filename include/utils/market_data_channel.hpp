#ifndef MARKET_DATA_CHANNEL_HPP
#define MARKET_DATA_CHANNEL_HPP

#include <mutex>
#include <condition_variable>
#include <memory>
#include "messaging/events/event.hpp"

class MarketDataChannel {
public:
    MarketDataChannel() : stop_requested_(false) {}

    // Usado pelo produtor (EventBus) para publicar o novo estado
    void update(std::shared_ptr<const Event> new_snapshot) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            latest_snapshot_ = std::move(new_snapshot);
        }
        cv_.notify_all(); // Notifica todos os consumidores
    }

    // Usado pelo consumidor (MarketDataGateway) para esperar por uma atualização
    // Retorna nullptr se o canal for desligado
    std::shared_ptr<const Event> wait_for_update() {
        std::unique_lock<std::mutex> lock(mtx_);

        // Dorme até que 'update' ou 'shutdown' seja chamado
        cv_.wait(lock, [this]{ return latest_snapshot_ != nullptr || stop_requested_; });
        
        if (stop_requested_) {
            return nullptr;
        }

        return std::move(latest_snapshot_);
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stop_requested_ = true;
        }
        cv_.notify_all();
    }

private:
    std::shared_ptr<const Event> latest_snapshot_ = nullptr;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_requested_;
};

#endif // MARKET_DATA_CHANNEL_HPP