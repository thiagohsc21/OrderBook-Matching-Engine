#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue 
{
public:
    ThreadSafeQueue() : stop_requested_(false) {}

    // Adiciona um item à fila e notifica um consumidor.
    void push(T item) 
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(item));
        }
        condition_.notify_one(); 
    }

    // Espera por um item e o retira da fila.
    // Retorna 'false' se a fila foi desligada e está vazia, indicando que o consumidor deve parar.
    bool wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // A thread vai dormir e liberar o mutex até que uma das duas condições seja verdadeira:
        // 1. A fila não está mais vazia.
        // 2. O desligamento foi solicitado.
        condition_.wait(lock, [this]{ return !queue_.empty() || stop_requested_; });

        // Após acordar, verificamos por que acordamos.
        // Se o desligamento foi solicitado E a fila estiver vazia, retornamos 'false'.
        if (stop_requested_ && queue_.empty()) 
        {
            return false;
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    // Novo método para sinalizar o desligamento da fila.
    void shutdown() 
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_requested_ = true;
        }
        // Notifica TODAS as threads que possam estar esperando para que elas reavaliem a condição.
        condition_.notify_all();
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

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_requested_;
};

#endif // THREAD_SAFE_QUEUE_HPP