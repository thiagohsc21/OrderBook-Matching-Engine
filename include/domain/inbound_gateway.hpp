#ifndef INBOUND_GATEWAY_HPP
#define INBOUND_GATEWAY_HPP

#include "messaging/commands/command.hpp"
#include "utils/thread_safe_queue.hpp" 
#include <string>
#include <memory>
#include <map>
#include <fstream> 

class InboundGateway 
{
public:
    explicit InboundGateway(ThreadSafeQueue<std::unique_ptr<Command>>& queue, const std::string& wal_file_path);

    void writeAheadLog(const std::string& log_message);
    bool pushToQueue(std::unique_ptr<Command> commandPtr);
    std::unique_ptr<Command> parseAndCreateCommand(const std::string& lines, const std::string& clientId, const std::chrono::system_clock::time_point& timestamp);
    std::unique_ptr<Command> createCommandFromFields(const std::map<std::string, std::string>& fields, const std::chrono::system_clock::time_point& timestamp);

private:
    ThreadSafeQueue<std::unique_ptr<Command>>& command_queue_;
    const std::string& wal_file_path_;
    std::ofstream wal_file_;
};

#endif // INBOUND_GATEWAY_HPP