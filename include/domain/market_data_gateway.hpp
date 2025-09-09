#ifndef MARKET_DATA_GATEWAY_HPP
#define MARKET_DATA_GATEWAY_HPP

#include "utils/market_data_channel.hpp"
#include <string>
#include <fstream>

class MarketDataGateway {
public:
    explicit MarketDataGateway(MarketDataChannel& channel, const std::string& output_file_path = "src/logs/market_data.log");
    bool initialize();
    void run();

private:
    std::string formatSnapshotToJSON(const Event& event);
    MarketDataChannel& channel_;
    std::string output_file_path_;
    std::ofstream output_file_;
};

#endif // MARKET_DATA_GATEWAY_HPP