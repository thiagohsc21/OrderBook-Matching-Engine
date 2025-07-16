#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <cstdint>
#include <memory>

class Engine;

class Command {
public:
    virtual ~Command() = default;
    
    // Todo comando DEVE implementar esta função. Ele recebe uma referência ao motor para poder chamar os métodos de negócio corretos
    virtual void execute(Engine& engine) = 0;
};

#endif // COMMAND_HPP