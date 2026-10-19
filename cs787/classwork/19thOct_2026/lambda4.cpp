#include <iostream>

// The function accepts the lambda via a generic template parameter 'F'
template <typename F>
void executeOperation(F operation) {
    // Make the call to the expression via the parameter
    int result = operation(10, 5); 
    std::cout << "Template Result: " << result << '\n';
}

int main() {
    // 1. Assign the lambda expression to a variable
    auto multiply = [](int a, int b) {
        return a * b;
    };

    // 2. Pass the variable as a parameter
    executeOperation(multiply); // Outputs: Template Result: 50

    return 0;
}
