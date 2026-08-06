// ====================== 23B1023 =========================

// client.cpp
// Builds on the socket basics from the class client.cpp, but now goes
// through the generated proxy instead of hand-rolling send()/recv().
//
// the client only ever touches a TAXCalculator*, and doesn't know
// (or care) that calculate() is actually going over the network.

#include <iostream>
#include "TAXCalculator.h"
#include "proxy.h"

int main() {
    TAXCalculator *remote = new TAXCalculatorProxy();
    static_cast<TAXCalculatorProxy*>(remote)->setReal("127.0.0.1", 9090);

    int income = 1000;
    int tax = remote->calculate(income);

    std::cout << "Tax on income " << income << " = " << tax << std::endl;

    delete remote;
    return 0;
}
