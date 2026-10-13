#pragma once

#include <array>
#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace cc {

using Date = std::chrono::sys_days;

// One day of data for one market.
struct Row {
    Date date;
    std::array<std::optional<double>, 4> prices ;
};

// All the data for one market.
struct MarketData {
    std::string name;
    std::vector<Row> rows;
};

MarketData load_csv(const std::string& name, const std::string& path);

}  // namespace cc