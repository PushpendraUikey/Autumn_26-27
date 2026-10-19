#include <iostream>

int main() {
    // 1. Assign the lambda expression to a variable using 'auto'
    auto add = [](auto a, auto b) -> auto {
        return a + b;
    };

    // 2. Use the variable in a call
    int result = add(5, 3);

    std::cout << "Result: " << result << '\n'; // Outputs: Result: 8
    std::cout << "2.5 + 3.1 = " << add(2.5, 3.1) << '\n'; 
    std::cout << "Hello + World = " << add(std::string("Hello "), std::string("World")) << '\n';

    return 0;
}

