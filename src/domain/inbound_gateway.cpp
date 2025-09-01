#include "domain/inbound_gateway.hpp"
#include "messaging/commands/new_order_command.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <filesystem> 


InboundGateway::InboundGateway(ThreadSafeQueue<std::unique_ptr<Command>>& queue, const std::string& wal_file_path)
    : command_queue_(queue), 
      wal_file_path_(wal_file_path) 
{
    // Move to InboundGateway::start() function after refactoring
    
    // Create directory if it doesn't exist
    std::filesystem::path file_path(wal_file_path);
    std::filesystem::path dir_path = file_path.parent_path();
    
    if (!dir_path.empty() && !std::filesystem::exists(dir_path)) 
    {
        try 
        {
            std::filesystem::create_directories(dir_path);
            std::cout << "Created directory: " << dir_path << '\n';
        } 
        catch (const std::filesystem::filesystem_error& e) 
        {
            std::cerr << "Failed to create directory: " << e.what() << '\n';
        }
    }
    
    // Open file with flags that create the file if it doesn't exist
    wal_file_.open(wal_file_path, std::ios::out | std::ios::app);
    if (!wal_file_.is_open()) 
    {
        std::cerr << "Failed to open WAL file: " << wal_file_path << '\n';
    }
    else
    {
        std::cout << "WAL file opened successfully: " << wal_file_path << '\n';
        // Clear the file content since we want to start fresh (trunc behavior)
        wal_file_.close();
        wal_file_.open(wal_file_path, std::ios::out | std::ios::trunc);
    }
}

void InboundGateway::writeAheadLog(const std::string& log_message) 
{   
    if (wal_file_.is_open()) 
    {
        wal_file_ << log_message << '\n';
    } 
    else 
    {
        std::cerr << "Failed to write log to WAL file as it's closed: " << wal_file_path_ << "\"'\n'";
    }
}

bool InboundGateway::pushToQueue(std::unique_ptr<Command> commandPtr) 
{   
    if (commandPtr) 
    {
        command_queue_.push(std::move(commandPtr));
        return true;
    } 
    else
    {
        std::cerr << "Failed to push command to queue: commandPtr is null.\n";
        return false;
    }
}

std::unique_ptr<Command> InboundGateway::parseAndCreateCommand(const std::string& line, const std::string& clientId, const std::chrono::system_clock::time_point& timestamp) 
{
    if (line.empty()) return nullptr;

    writeAheadLog(line);

    std::stringstream oss(line);
    std::string tag;

    std::map<std::string, std::string> fix_fields;
    fix_fields["1"] = clientId; 

    while (std::getline(oss, tag, '|')) 
    {
        std::size_t pos = tag.find('=');
        if (pos != std::string::npos) 
        {
            std::string key = tag.substr(0, pos);
            std::string value = tag.substr(pos + 1);
            fix_fields[key] = value;
        }
    }

    if(fix_fields.size() < 2) 
    {
        std::cerr << "No valid FIX fields found\n";
        return nullptr;
    }

    return createCommandFromFields(fix_fields, timestamp);
}

std::unique_ptr<Command> InboundGateway::createCommandFromFields(const std::map<std::string, std::string>& fields, const std::chrono::system_clock::time_point& timestamp) 
{
    uint64_t client_order_id = 0;
    uint64_t client_id = 0;
    std::string symbol;
    OrderSide side;
    OrderType type;
    uint32_t quantity = 0;
    double price = 0.0;
    std::string ordType;
    std::string timeInForce;
    std::string orderCapacity;

    try 
    {
        client_order_id = std::stoull(fields.at("11"));
        client_id = std::stoull(fields.at("1"));
        symbol = fields.at("55");
        side = static_cast<OrderSide>(std::stoi(fields.at("54")));
        type = static_cast<OrderType>(std::stoi(fields.at("40")));
        quantity = std::stoul(fields.at("38"));
        price = std::stod(fields.at("44"));
        ordType = fields.at("40");          
        timeInForce = fields.at("59");     
        orderCapacity = fields.at("47");   
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Erro ao converter campos do FIX: " << e.what() << "\n";
        return nullptr;
    }

    return std::make_unique<NewOrderCommand>(
        client_order_id, client_id, symbol, side, type, quantity, price, 
        static_cast<OrderTimeInForce>(std::stoi(timeInForce)), static_cast<OrderCapacity>(orderCapacity[0]),
        timestamp 
    );
}