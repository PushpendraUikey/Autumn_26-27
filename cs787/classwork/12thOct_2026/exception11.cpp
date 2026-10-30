#include <iostream>
#include <stdexcept>
using namespace std;

void f() try {
    throw runtime_error("Original exception");
}
catch (const exception& e) {
    cout << "f handler: " << e.what() << '\n';
    // No explicit throw
}

int main() {
    try {
        f();
        cout << "f returned normally\n";
    }
    catch (...) {
        cout << "main caught exception\n";
    }
}

// runtime_error is a standard C++ exception class used to report errors that occur during 
// program execution and cannot be conveniently represented by other standard exception types.
// It is declared in the <stdexcept> header.

// function try blocks can also be used for ordinary functions, but they are mainly meant for 
// constructors. Function try blocks don't rethrow the exception automatically as in case of 
// constructors try blocks.

