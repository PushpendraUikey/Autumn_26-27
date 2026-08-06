// ====================== 23B1023 =========================

#ifndef TAXCALCULATOR_H
#define TAXCALCULATOR_H

// This is the abstract interface both the real (server-side) object
// and the proxy (client-side) object implement, exactly like in
// abstract.cpp from class. Client code only ever talks to a
// TAXCalculator*, so it can't tell whether it holds a local object
// or a proxy that goes over the network.

class TAXCalculator {
public:
    virtual int calculate(int) = 0;
    virtual ~TAXCalculator() {}
};

#endif
