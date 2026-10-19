#include <iostream>
#include <functional> // Required for std::function

// The function specifies the exact signature: std::function<ReturnType(Args...)>
void executeOperationWithFunction(std::function<int(int, int)> operation) {
    // Make the call to the expression via the parameter
    int result = operation(10, 5); 
    std::cout << "std::function Result: " << result << '\n';
}

int main() {
    auto subtract = [](int a, int b) {
        return a - b;
    };

    executeOperationWithFunction(subtract); // Outputs: std::function Result: 5
    return 0;
}
