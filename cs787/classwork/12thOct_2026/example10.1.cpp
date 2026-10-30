#include <iostream>
#include <stdexcept>
using namespace std;

class A {
public:
    A() try {
        throw runtime_error("Original exception");
    }
    catch (const exception& e) {
        cout << "A handler: " << e.what() << '\n';
        throw logic_error("New exception from handler");
    }
};

int main() {
    try {
        A a;
    }
    catch (const exception& e) {
        cout << "main: " << e.what() << '\n';
    }
}
