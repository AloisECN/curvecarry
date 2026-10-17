#include <array>
#include <chrono>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <string>


#include "marketdata.hpp"

// In the omnisiah we pray, so that the machine spirit be mercifull
int main() {
    using namespace std::chrono_literals;

    try {
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

            //Quick print to see if isokay
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
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}