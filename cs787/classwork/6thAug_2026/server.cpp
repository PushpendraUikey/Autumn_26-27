// ====================== 23B1023 =========================

// server.cpp
// Builds on the socket basics from the class server.cpp, but the
// accept/recv/dispatch/send loop now lives in the generated
// serve_TAXCalculator() (stuff.cpp) instead of being written by hand
// here. This file only has to provide the REAL implementation --
// same TAXCalculatorReal as in abstract.cpp.

#include "TAXCalculator.h"
#include "stuff.h"

class TAXCalculatorReal : public TAXCalculator {
public:
    int calculate(int income) override {
        return (int)(0.1 * income);
    }
};

int main() {
    TAXCalculatorReal real;
    serve_TAXCalculator(&real, 9090);   // never returns
    return 0;
}
