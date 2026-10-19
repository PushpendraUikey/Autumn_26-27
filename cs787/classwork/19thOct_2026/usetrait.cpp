#include <iostream>
#include <type_traits>
using namespace std;


//g++ usetrait.cpp -fconcepts
// Define a template that ONLY accepts integral types
template <typename T>

// Requires clause attaches a compile-time boolean condition (such as a concept or type trait) to a template.
requires std::is_integral_v<T> 
void print_number(T val) {
    std::cout << "Integral: " << val << "\n";
}

int main() {
    print_number(42);       // Works!
    print_number('A');      // Works! (char is integral)
    print_number(true);     // Works! (bool is integral)
    print_number(100L);    // Works! (long is integral)
    // print_number(3.14);     // Compile error: Constraints not satisfied
    // print_number(2.718f);   // Compile error: Constraints not satisfied
    // print_number("42");  // Compile error: Constraints not satisfied
}
