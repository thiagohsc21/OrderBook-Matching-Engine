#ifndef EVENT_HPP
#define EVENT_HPP

#include <chrono>

class Event 
{
    // Um evento é uma classe base que pode ser estendida para criar eventos específicos
    // Ele contém um timestamp que indica quando o evento foi gerado
    // As classes filhas devem implementar a lógica específica do evento
public:
    virtual ~Event() = default;

    // Cada evento deve ter um nome único para identificação
    virtual const char* getEventName() const = 0;
    
    // Todo evento tem um timestamp de quando foi gerado
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }

protected:
    // O construtor é protegido para que apenas as classes filhas possam chamá-lo
    Event() : timestamp_(std::chrono::system_clock::now()) {}

private:
    std::chrono::system_clock::time_point timestamp_;
};

#endif // EVENT_HPP