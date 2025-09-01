#include "domain/auditor.hpp"
#include "messaging/events/order_accepted_event.hpp"
#include "messaging/events/trade_executed_event.hpp"
#include "utils/timestamp_formatter.hpp"
#include "domain/order.hpp"
#include "domain/trade.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <typeinfo>

Auditor::Auditor(ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue, const std::string& log_file_path)
    : event_queue_(event_queue), 
      log_file_path_(log_file_path)
{
}

bool Auditor::initialize()
{
    // Criar diretório se não existir
    std::filesystem::path file_path(log_file_path_);
    std::filesystem::path dir_path = file_path.parent_path();
    
    if (!dir_path.empty() && !std::filesystem::exists(dir_path)) 
    {
        try 
        {
            std::filesystem::create_directories(dir_path);
            std::cout << "Created auditor log directory: " << dir_path << '\n';
        } 
        catch (const std::filesystem::filesystem_error& e) 
        {
            std::cerr << "Failed to create auditor log directory: " << e.what() << '\n';
            return false;
        }
    }
    
    // Abrir arquivo de log com flags que criam o arquivo se não existir
    log_file_.open(log_file_path_, std::ios::out | std::ios::app);
    if (!log_file_.is_open()) 
    {
        std::cerr << "Failed to open auditor log file: " << log_file_path_ << '\n';
        return false;
    }
    else
    {
        std::cout << "Auditor log file opened successfully: " << log_file_path_ << '\n';
        // Clear the file content since we want to start fresh (trunc behavior)
        log_file_.close();
        log_file_.open(log_file_path_, std::ios::out | std::ios::trunc);
    }
    
    return true;
}

void Auditor::run() 
{
    while (true) 
    {
        std::shared_ptr<const Event> event;
        
        // wait_and_pop bloqueia até que haja um item OU a fila seja desligada
        // Se wait_and_pop retornar false, significa que a fila foi desligada e está vazia, thread deve terminar
        if (!event_queue_.wait_and_pop(event)) {
            break; 
        }
        
        // Se chegamos aqui, temos um evento válido e devemos logá-lo - Persistir em um banco ou arquivo
        writeEventLog(event);
    }
    std::cout << "Auditor has finished consuming." << std::endl;
    
    // Fechar arquivo de log ao terminar
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Auditor::writeEventLog(const std::shared_ptr<const Event>& event)
{
    if (!log_file_.is_open()) {
        std::cerr << "Cannot write event log: log file is not open" << std::endl;
        return;
    }
    
    std::string eventType = "Unknown";
    std::string eventDetails = "";
    
    if (auto orderEvent = std::dynamic_pointer_cast<const OrderAcceptedEvent>(event)) {
        eventType = "OrderAccepted";
        std::stringstream details;
        details << "OrderID:" << orderEvent->getOrderId() 
               << " | ClientID:" << orderEvent->getClientId()
               << " | ClientOrderID:" << orderEvent->getClientOrderId()
               << " | Symbol:" << orderEvent->getSymbol()
               << " | Side:" << static_cast<int>(orderEvent->getSide())
               << " | Quantity:" << orderEvent->getQuantity()
               << " | Price:" << orderEvent->getPrice();
        eventDetails = details.str();
    }
    else if (auto tradeEvent = std::dynamic_pointer_cast<const TradeExecutedEvent>(event)) {
        eventType = "TradeExecuted";
        std::stringstream details;
        details << "TradeID:" << tradeEvent->getTradeId()
               << " | Symbol:" << tradeEvent->getSymbol()
               << " | Quantity:" << tradeEvent->getQuantity()
               << " | Price:" << tradeEvent->getPrice()
               << " | AggressiveOrderID:" << tradeEvent->getAggressiveOrderId()
               << " | PassiveOrderID:" << tradeEvent->getPassiveOrderId()
               << " | AggressiveRemainingQty:" << tradeEvent->getAggressiveRemainingQuantity()
               << " | PassiveRemainingQty:" << tradeEvent->getPassiveRemainingQuantity();
        eventDetails = details.str();
    }
    else {
        // Evento genérico
        eventType = event->getEventName();
        eventDetails = "Generic event processed";
    }
    
    log_file_ << TimestampFormatter::format(event->getTimestamp()) << " - " 
              << eventType << " - " 
              << "\"" << eventDetails << "\"\n";
}