#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "marketdata.hpp"
#include "roll_rule.hpp"

// In the omnisiah we pray, so that the machine spirit be mercifull
int main() {
    using namespace std::chrono_literals;

    try {
        const cc::PriceBasedRoll price_rule;
        const cc::CalendarRoll calendar_rule;

        for (const std::string name : {"wti", "ho", "rbob", "ng"}) {
            auto md = cc::load_csv(name, "data/" + name + ".csv");

            std::array<int, 4> missing{};
            for (const auto& row : md.rows)
                for (std::size_t i = 0; i < 4; ++i)
                    if (!row.prices[i]) ++missing[i];

            std::cout << std::format("{}: {} rows, {:%F} -> {:%F}, missing C1-C4: {} {} {} {}\n",
                                     md.name, md.rows.size(),
                                     md.rows.front().date, md.rows.back().date,
                                     missing[0], missing[1], missing[2], missing[3]);

            if (name == "wti") {
                const cc::Date target = std::chrono::sys_days{2020y / 4 / 20};
                for (const auto& row : md.rows) {
                    if (row.date == target) {
                        if (row.prices[0])
                            std::cout << "  WTI C1 on 2020-04-20: " << *row.prices[0] << "\n";
                        else
                            std::cout << "  WTI C1 on 2020-04-20: missing\n";
                    }
                }
            }

            const auto price_flags = price_rule.detect(md);
            //const auto calendar_flags = calendar_rule.detect(md);
            std::vector<bool> calendar_flags(price_flags.size(), false);

            const auto n_price = std::count(price_flags.begin(), price_flags.end(), true);
            const auto n_calendar = std::count(calendar_flags.begin(), calendar_flags.end(), true);
            const auto diffs = cc::compare_flags(price_flags, calendar_flags);

            std::cout << std::format("  rolls: price-based {}, calendar {}, disagreements {}\n",
                                     n_price, n_calendar, diffs.size());

            std::cout << "  first flagged dates:";
            int shown = 0;
            for (std::size_t i = 0; i < price_flags.size() && shown < 10; ++i) {
                if (price_flags[i]) {
                    std::cout << std::format(" {:%F}", md.rows[i].date);
                    ++shown;
                }
            }
            std::cout << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}