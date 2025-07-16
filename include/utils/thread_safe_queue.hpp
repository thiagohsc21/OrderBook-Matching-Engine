#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <memory>
#include <iostream>
#include <condition_variable> // Adicione isto
#include "messaging/command.hpp"
#include "messaging/new_order_command.hpp"

template<typename T>
class ThreadSafeQueue 
{
public:
    void push(T item) 
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(item));
        }
        cond_var_.notify_one(); // Notifica um consumidor esperando
    }

    bool try_pop(T& value) 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) 
        {
            std::cerr << "Queue is empty, cannot pop.\n";
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this]{ return !queue_.empty(); });
        value = std::move(queue_.front());
        queue_.pop();
    }

    bool empty() const 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void print_and_clear() 
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (queue_.empty()) 
        {
            std::cout << "Queue is empty.\n";
            return;
        }

        std::cout << "Queue contents:\n";
        while (!queue_.empty()) 
        {
            const auto& ptr = queue_.front();
            if (ptr) 
            {
                auto* order = dynamic_cast<NewOrderCommand*>(ptr.get());
                if (order) 
                {
                    std::cout << "ClientID: " << order->getClientId()
                            << ", ClientOrderID: " << order->getClientOrderId()
                            << ", Symbol: " << order->getSymbol()
                            << ", Quantity: " << order->getQuantity() << "\n";
                } 
                else 
                {
                    std::cout << "Comando desconhecido\n";
                }
            } 
            else 
            {
                std::cout << "nullptr\n";
            }
            queue_.pop();
        }
}

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_; 
};

#endif // THREAD_SAFE_QUEUE_HPP