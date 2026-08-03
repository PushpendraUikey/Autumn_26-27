#include <bits/stdc++.h>
using namespace std;

class TAXCalculator {
    public:
        virtual int calculate(int) = 0;
};

class TAXCalculatorReal : public TAXCalculator {
    public:
        int calculate(int income) override {
            return (income * 0.1);
        }
};

class TAXCalculatorProxy : public TAXCalculator {
    public:
        TAXCalculator *real;
        void setReal(TAXCalculator *tc) {
            real = tc;
        }
        int calculate(int income) override {
            return real->calculate(income);
        }
}
;

int main() {
    TAXCalculator *remote;
    TAXCalculatorProxy *local;
        remote = new TAXCalculatorReal();
        local = new TAXCalculatorProxy();
        local->setReal(remote);
        cout << local->calculate(15000) << endl;
}