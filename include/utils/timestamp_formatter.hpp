#ifndef TIMESTAMP_FORMATTER_HPP
#define TIMESTAMP_FORMATTER_HPP

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

class TimestampFormatter {
public:
    static std::string format(const std::chrono::system_clock::time_point& timestamp) {
        // Converte o timestamp para time_t
        std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;

        // Formata a parte de data e hora
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

        // Extrai os nanosegundos
        auto duration = timestamp.time_since_epoch();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000000000;

        // Adiciona os nanosegundos ao timestamp
        ss << "." << std::setfill('0') << std::setw(9) << nanoseconds;

        return ss.str();
    }
};

#endif // TIMESTAMP_FORMATTER_HPP