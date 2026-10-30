
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
        // No explicit throw
    }
};

int main() {
    try {
        A a;
        cout << "Object constructed\n";
    }
    catch (const exception& e) {
        cout << "main: " << e.what() << '\n';
    }

    cout << "Program continues\n";
}
