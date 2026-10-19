#include <functional>
#include <iostream>

void greet() {
    std::cout << "Hello, World!" << std::endl;
}

int main() {
    std::function<void()> func = greet; // std::function is a general purpose polymorphic function wrapper
    func(); // Calls greet()
    return 0;
}
