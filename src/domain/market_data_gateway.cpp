#include "domain/market_data_gateway.hpp"
#include "messaging/events/book_snapshot_event.hpp" 
#include "utils/timestamp_formatter.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <typeinfo>
#include <fstream>

MarketDataGateway::MarketDataGateway(MarketDataChannel& channel, const std::string& output_file_path) 
    : channel_(channel), output_file_path_(output_file_path) 
{
}

bool MarketDataGateway::initialize()
{
    // Criar diretório se não existir
    std::filesystem::path file_path(output_file_path_);
    std::filesystem::path dir_path = file_path.parent_path();
    
    if (!dir_path.empty() && !std::filesystem::exists(dir_path)) 
    {
        try 
        {
            std::filesystem::create_directories(dir_path);
            std::cout << "Created market data log directory: " << dir_path << '\n';
        } 
        catch (const std::filesystem::filesystem_error& e) 
        {
            std::cerr << "Failed to create market data log directory: " << e.what() << '\n';
            return false;
        }
    }
    
    // Abrir arquivo de saída com flags que criam o arquivo se não existir
    output_file_.open(output_file_path_, std::ios::out | std::ios::app);
    if (!output_file_.is_open()) 
    {
        std::cerr << "Failed to open market data output file: " << output_file_path_ << '\n';
        return false;
    }
    else
    {
        std::cout << "Market data output file opened successfully: " << output_file_path_ << '\n';
        output_file_.close();
        output_file_.open(output_file_path_, std::ios::out | std::ios::trunc);
    }
    
    return true;
}

void MarketDataGateway::run() {

    std::cout << "[MarketDataGateway] Thread started. Waiting for market data..." << std::endl;

    while (true) {
        std::shared_ptr<const Event> event = channel_.wait_for_update();
        
        if (!event) {
            break;
        }
        
        std::string json_output = formatSnapshotToJSON(*event);
        
        if (output_file_.is_open()) {
            output_file_ << json_output << "\n";
        } else {
            std::cerr << "[MarketDataGateway] Cannot write market data: output file is not open" << std::endl;
        }
    }

    if (output_file_.is_open()) {
        output_file_.close();
    }

    std::cout << "MarketDataGateway has finished consuming." << std::endl;
}

std::string MarketDataGateway::formatSnapshotToJSON(const Event& event) {

    const auto* snapshot = dynamic_cast<const BookSnapshotEvent*>(&event);
    if (!snapshot) {
        return "{ \"error\": \"Unknown market data event type\" }";
    }

    std::stringstream ss;
    ss << "{\n";
    ss << "  \"symbol\": \"" << snapshot->getSymbol() << "\",\n";
    ss << "  \"timestamp\": \"" << TimestampFormatter::format(snapshot->getTimestamp()) << "\",\n";
    
    ss << "  \"bids\": [\n";
    for (size_t i = 0; i < snapshot->getBids().size(); ++i) {
        ss << "    { \"price\": " << snapshot->getBids()[i].price 
           << ", \"quantity\": " << snapshot->getBids()[i].quantity << " }";
        if (i < snapshot->getBids().size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "  ],\n";
    
    ss << "  \"asks\": [\n";
    for (size_t i = 0; i < snapshot->getAsks().size(); ++i) {
        ss << "    { \"price\": " << snapshot->getAsks()[i].price 
           << ", \"quantity\": " << snapshot->getAsks()[i].quantity << " }";
        if (i < snapshot->getAsks().size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}";

    return ss.str();
}