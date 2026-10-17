#pragma once

#include <vector>
#include "marketdata.hpp"

namespace cc {
class RollRule {
public:
    virtual ~RollRule() = default;

    // Input: the market data (by const reference). Output: one flag per row.
    // flag[i] is true if day i is a roll day. Day 0 can never be one.
    virtual std::vector<bool> detect(const MarketData& md) const = 0;

};

//Overide clasess 
class PriceBasedRoll : public RollRule {
public :
    std::vector<bool> detect(const MarketData& md) const override; 
};

class CalendarRoll : public RollRule {
public :
    std::vector<bool> detect(const MarketData& md) const override; 
};

// free functions compare flags
// Input: two flag vectors
// Output: the indices where they disagree.
std::vector<int> compare_flags(const std::vector<bool>& flag1,
                            const std::vector<bool>& flag2);


}  // namespace cc