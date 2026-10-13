#include <marketdata.hpp> 

#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace cc {


// Source - https://stackoverflow.com/a/14266139
// Posted by Vincenzo Pii, modified by community. See post 'Timeline' for change history
// Retrieved 2026-10-13, License - CC BY-SA 4.0

static std::vector<std::string> split(std::string s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

// My code below
static Date parse_date(const std::string& s) {
    std::string delimiter = "-";
    auto parts = split(s , delimiter) ;
    //handles maleformed date
    if (parts.size() != 3){
        throw std::runtime_error(
            "Expected date, got " + std::to_string(parts.size())
        );
    }
    int year  = std::stoi(parts[0]);
    int month = std::stoi(parts[1]);
    int day   = std::stoi(parts[2]);

    auto ymd = std::chrono::year_month_day{
        std::chrono::year{year},
        std::chrono::month{static_cast<unsigned>(month)},
        std::chrono::day{static_cast<unsigned>(day)}
    };

    return std::chrono::sys_days{ymd};
}

MarketData load_csv(const std::string& name, const std::string& path) {
    std::ifstream file_in(path); // path is the full file path, e.g. "data/wti.csv"
    if (!file_in) {
        throw std::runtime_error("cannot open " + path);
    }

    MarketData md;
    md.name = name;

    std::string line;
    std::getline(file_in, line);// discard header

    while (std::getline(file_in, line)) {
        if (line.empty()) continue;

        auto parts = split(line, ",");
        // A well-formed row is a date plus 4 prices 
        if (parts.size() != 5) {
            throw std::runtime_error("bad row in " + path + ": " + line);
        }

        Row row;
        row.date = parse_date(parts[0]);

        // Dates must be strictly increasing: later code relies on it
        if (!md.rows.empty() && row.date <= md.rows.back().date) {
            throw std::runtime_error("dates not ascending in " + path + ": " + line);
        }

        // parts[0] is the date, so contract i+1 sits at parts[i + 1]
        for (std::size_t i = 0; i < 4; ++i) {
            const std::string& cell = parts[i + 1];
            if (cell.empty())
                row.prices[i] = std::nullopt;         // no quote that day
            else
                row.prices[i] = std::stod(cell);      // negative prices !
        }

        md.rows.push_back(row);
    }
    return md;
}

}  // namespace cc