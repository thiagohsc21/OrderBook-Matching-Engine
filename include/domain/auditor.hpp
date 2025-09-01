#ifndef AUDITOR_HPP
#define AUDITOR_HPP

#include "messaging/events/event.hpp"
#include "utils/thread_safe_queue.hpp"
#include <string>
#include <memory>
#include <fstream>

class Auditor {
public:
    explicit Auditor(ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue, const std::string& log_file_path = "src/logs/auditor_log/events.log");
    bool initialize();
    void run();

private:
    ThreadSafeQueue<std::shared_ptr<const Event>>& event_queue_;
    std::string log_file_path_;
    std::ofstream log_file_;
    
    void writeEventLog(const std::shared_ptr<const Event>& event);
};

#endif 