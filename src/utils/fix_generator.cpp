#include "utils/fix_generator.hpp"
#include "types/order_params.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <chrono>

// Static member initialization
int FixGenerator::seqnum_ = 0;
std::mutex FixGenerator::seq_mutex_;

std::string FixGenerator::calculateChecksum(const std::string& msg) 
{
    int sum = 0;
    for (char c : msg)
    {
        sum += static_cast<unsigned char>(c);
    }

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << (sum % 256); // EX: sum = 5 => oss = "005"

    return oss.str();
}

std::string FixGenerator::generateFIXMessage(const std::string& symbol, int quantity, double price, int side, const std::string& msgType, 
                                             const std::string& targetCancelId, const std::string& ordType, const std::string& timeInForce, const std::string& orderCapacity)
{
    std::lock_guard<std::mutex> lock(seq_mutex_);
    seqnum_++;
    std::ostringstream oss;

    std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    oss << "8=FIX.4.2|"      // 8 = BeginString (Versão do protocolo FIX)
        << "49=CLIENT|"      // 49 = SenderCompID (Identificador do remetente)
        << "56=SERVER|"      // 56 = TargetCompID (Identificador do destinatário)
        << "34=" << seqnum_ << "|" // 34 = MsgSeqNum (Número sequencial da mensagem)
        << "52=" << std::put_time(std::localtime(&now_time), "%Y%m%d-%H:%M:%S") << "|"; // 52 = SendingTime (Data/hora de envio)

    if (msgType == "D") 
    {
        oss << "35=D|"        // 35 = MsgType (Tipo da mensagem: D = New Order - Single)
            << "11=" << seqnum_ << "|" // 11 = ClOrdID (ID único da ordem do cliente)
            << "55=" << symbol << "|"  // 55 = Symbol (Símbolo do ativo)
            << "54=" << side << "|"    // 54 = Side (Lado: 1=Compra, 2=Venda)
            << "38=" << quantity << "|"// 38 = OrderQty (Quantidade da ordem)
            << "44=" << std::fixed << std::setprecision(2) << price << "|" // 44 = Price (Preço)
            << "40=" << ordType << "|" // 40 = OrdType (Tipo de ordem: 1=Market, 2=Limit, 3=Stop)
            << "59=" << timeInForce << "|" // 59 = TimeInForce (Tempo em vigor: 1=Day, etc)
            << "47=" << orderCapacity << "|"; // 47 = OrderCapacity (Capacidade: 1=Agência, 2=Principal)
    } 
    else if (msgType == "F") 
    {
        oss << "35=F|"        // 35 = MsgType (Tipo da mensagem: F = Cancelamento de ordem)
            << "11=" << seqnum_ << "|" // 11 = ClOrdID (ID único da ordem do cliente)
            << "41=" << targetCancelId << "|" // 41 = OrigClOrdID (ID da ordem original a ser cancelada)
            << "55=" << symbol << "|"  // 55 = Symbol (Símbolo do ativo)
            << "54=" << side << "|";   // 54 = Side (Lado: 1=Compra, 2=Venda)
    }

    std::string body = oss.str();
    body += "10=" + calculateChecksum(body) + "|"; // 10 = CheckSum (Verificação de integridade)
    
    return body;
}

std::string FixGenerator::generateFIXMessageForThread() 
{
    static thread_local std::default_random_engine rng(std::random_device{}());
    static thread_local std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG", "AMZN", "TSLA", "NVDA", "META", "NFLX", "AMD", "INTC", "BABA", "ORCL", "IBM"};
    static thread_local std::vector<std::string> msgTypes = {"D"}; // depois adicionar "F" para cancelamento e "G" para modificação
    static thread_local std::vector<std::string> orderTypes = {"1", "2"}; // 1=Market, 2=Limit, 3=Stop
    static thread_local std::vector<std::string> tif = {"1", "2", "3", "4"}; // 1=Day, 2=Good Till Cancel, 3=Immediate or Cancel, 4=Fill or Kill
    static thread_local std::vector<std::string> orderCapacities = {"1", "2"}; // 1=Agency, 2=Principal

    std::uniform_int_distribution<int> quantity_dist(1, 500);
    std::uniform_real_distribution<double> price_dist(10.0, 500.0);
    std::uniform_int_distribution<int> side_dist(1, 2);
    std::uniform_int_distribution<int> symbol_dist(0, symbols.size() - 1);
    std::uniform_int_distribution<int> type_dist(0, msgTypes.size() - 1);
    std::uniform_int_distribution<int> ordtype_dist(0, orderTypes.size() - 1);
    std::uniform_int_distribution<int> tif_dist(0, tif.size() - 1);
    std::uniform_int_distribution<int> orderCapacity_dist(0, orderCapacities.size() - 1);

    std::string msgType = msgTypes[type_dist(rng)];
    std::string symbol = symbols[symbol_dist(rng)];
    int quantity = quantity_dist(rng);
    double price = price_dist(rng);
    int side = side_dist(rng);
    std::string ordType = orderTypes[ordtype_dist(rng)];
    std::string timeInForce = tif[tif_dist(rng)];
    std::string orderCapacity = orderCapacities[orderCapacity_dist(rng)];

    // Passe o tipo de ordem (campo 40) e capacidade (campo 47) para a mensagem FIX
    return FixGenerator::generateFIXMessage(symbol, quantity, price, side, msgType, "", ordType, timeInForce, orderCapacity);
}

