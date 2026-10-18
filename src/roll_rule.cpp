#include "roll_rule.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace cc {

std::vector<bool> PriceBasedRoll::detect(const MarketData& md) const {
    const std::size_t n = md.rows.size();
    std::vector<bool> output(n, false);

    for (std::size_t i = 1; i < n; ++i) {
        const auto& today = md.rows[i];
        const auto& prev = md.rows[i - 1];

        if (!today.prices[0] || !today.prices[1] ||
            !prev.prices[0] || !prev.prices[1] || !prev.prices[2]) {
            continue;
        }

        const double spread_today = *today.prices[1] - *today.prices[0];
        const double spread_no_roll = *prev.prices[1] - *prev.prices[0];
        const double spread_roll = *prev.prices[2] - *prev.prices[1];

        if (std::abs(spread_today - spread_roll) < std::abs(spread_today - spread_no_roll)) {
            output[i] = true;
        }
    }
    return output;
}

std::vector<bool> CalendarRoll::detect(const MarketData& md) const {
    using namespace std::chrono;

    if (md.name != "wti") {
        throw std::invalid_argument("CalendarRoll only implements the WTI rule, got: " + md.name);
    }

    const std::size_t n = md.rows.size();
    std::vector<bool> output(n, false);
    if (n == 0) return output;

    const year_month_day first{md.rows.front().date};
    const year_month_day last{md.rows.back().date};
    year_month ym{first.year(), first.month()};
    const year_month ym_last{last.year(), last.month()};

    for (; ym <= ym_last; ym += months{1}) {
        const sys_days target{ym / 25};
        if (target > md.rows.back().date) break;

        const auto it = std::upper_bound(
            md.rows.begin(), md.rows.end(), target,
            [](const sys_days& d, const Row& r) { return d < r.date; });
        const auto count = static_cast<std::size_t>(it - md.rows.begin());
        if (count < 4) continue;

        const year_month_day ref{md.rows[count - 1].date};
        if (year_month{ref.year(), ref.month()} != ym) continue;

        output[count - 3] = true;
    }
    return output;
}

std::vector<int> compare_flags(const std::vector<bool>& flag1,
                               const std::vector<bool>& flag2) {
    const std::size_t n1 = flag1.size();
    const std::size_t n2 = flag2.size();

    if (n1 != n2) {
        throw std::invalid_argument("flag1 and flag2 have different sizes: " +
                                    std::to_string(n1) + " vs " +
                                    std::to_string(n2));
    }

    std::vector<int> out;
    for (std::size_t i = 0; i < n1; ++i) {
        if (flag1[i] != flag2[i]) {
            out.push_back(static_cast<int>(i));
        }
    }
    return out;
}

}  // namespace cc