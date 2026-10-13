#include <iostream>
#include<marketdata.hpp>

// In the omnisiah we pray, so that the machine spirit be mercifull
int main(){
    auto md = cc::load_csv("wti", "data/wti.csv");
    return 0 ;
}