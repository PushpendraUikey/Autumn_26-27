// ====================== 23B1023 =========================
//
// Demo.h -- hand-written interface matching demo.spec (test fixture).
// Slides 32/39 call this "Foo.h": shared by client and server.

#ifndef DEMO_H
#define DEMO_H

#include <string>

class Demo {
public:
    virtual int calculate(int) = 0;
    virtual int add(int, int) = 0;
    virtual std::string greet(std::string) = 0;
    virtual int ping() = 0;
    virtual void reset() = 0;
    virtual ~Demo() {}
};

#endif
