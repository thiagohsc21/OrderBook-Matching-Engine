#ifndef FIX_GENERATOR_HPP
#define FIX_GENERATOR_HPP

#include <string>
#include <mutex>
#include <vector>
#include <random>

// Variáveis Static são compartilhadas entre todas as instâncias da classe
// e entre todas as threads que usam essa classe.

// Métodos Static são funções que pertencem à classe, não a uma instância específica.
// Cada thread que chama um método static executa sua própria instância da função.
// As variáveis LOCAIS dentro do método são privadas de cada thread.

// Apenas variáveis STATIC da classe (como seqnum_) são compartilhadas entre threads.

class FixGenerator 
{
public:
static std::string generateFIXMessage(const std::string& symbol, int quantity, double price, int side, const std::string& msgType, 
                                      const std::string& targetCancelId = "", const std::string& orderType = "1", const std::string& timeInForce = "0", const std::string& orderCapacity = "1");
    static std::string generateFIXMessageForThread();

private:
    static int seqnum_;
    static std::mutex seq_mutex_;
    static std::string calculateChecksum(const std::string& msg);
};

#endif // FIX_GENERATOR_HPP