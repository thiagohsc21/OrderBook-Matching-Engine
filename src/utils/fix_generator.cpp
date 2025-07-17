#include "utils/fix_generator.hpp"
#include "utils/timestamp_formatter.hpp"
#include "types/order_params.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <random>

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

std::pair<std::string, std::chrono::system_clock::time_point> FixGenerator::generateFIXMessage(
    const std::string& symbol, int quantity, double price, int side, const std::string& msgType, 
    const std::string& targetCancelId, const std::string& ordType, const std::string& timeInForce, const std::string& orderCapacity)
{
    std::lock_guard<std::mutex> lock(seq_mutex_);
    seqnum_++;
    std::ostringstream oss;

    // Gera o timestamp atual
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::string formatted_time = TimestampFormatter::format(now);

    oss << "8=FIX.4.2|"
        << "49=CLIENT|"
        << "56=SERVER|"
        << "34=" << seqnum_ << "|"
        << "52=" << formatted_time << "|";

    if (msgType == "D") {
        oss << "35=D|"
            << "11=" << seqnum_ << "|"
            << "55=" << symbol << "|"
            << "54=" << side << "|"
            << "38=" << quantity << "|"
            << "44=" << std::fixed << std::setprecision(2) << price << "|"
            << "40=" << ordType << "|"
            << "59=" << timeInForce << "|"
            << "47=" << orderCapacity << "|";
    }

    std::string body = oss.str();
    body += "10=" + calculateChecksum(body) + "|";

    return {body, now}; // Retorna a mensagem FIX e o timestamp gerado
}

std::pair<std::string, std::chrono::system_clock::time_point> FixGenerator::generateFIXMessageForThread() 
{
    static thread_local std::default_random_engine rng(std::random_device{}());
    static thread_local std::vector<std::string> symbols = {"AAPL", "AMZN", "GOOG"};
    static thread_local std::vector<std::string> msgTypes = {"D"};
    static thread_local std::vector<std::string> orderTypes = {"1", "2"};
    static thread_local std::vector<std::string> tif = {"1", "2", "3", "4"};
    static thread_local std::vector<std::string> orderCapacities = {"A", "P"};

    std::uniform_int_distribution<int> quantity_dist(1, 100);
    std::uniform_real_distribution<double> price_dist(10.0, 10.50);
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

    return FixGenerator::generateFIXMessage(symbol, quantity, price, side, msgType, "", ordType, timeInForce, orderCapacity);
}
