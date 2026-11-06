#include <iostream>
#include <variant>
#include <string>

int main() {
    //1. Declaration (Defaults to the first type initialized)
    std::variant<int, double, std::string> myVariant;

    //2. Initialization
    myVariant = 42; // Now holds an int
    std::cout << "Current value: " << std::get<int>(myVariant) << std::endl;
    myVariant = 3.14; // Now holds a double
    std::cout << "Current value: " << std::get<double>(myVariant) << std::endl;
    myVariant = "Hello, Pushpendra"; // Now holds a string
    std::cout << "Current value: " << std::get<std::string>(myVariant) << std::endl;

    // 3. Inspection: Check what type is currently active
    std::cout << "Active index: " << myVariant.index() << "\n"; // Returns 2 (0-indexed list)

    if (std::holds_alternative<std::string>(myVariant)) {
        std::cout << "It's a string!\n";
    }

    // 4. Access Method A: std::get (Throws exception if wrong)
    try {
        std::cout << std::get<std::string>(myVariant) << "\n";
        std::get<int>(myVariant); // This would throw std::bad_variant_access!
    } catch (const std::bad_variant_access& ex) {
        std::cout << "Wrong type accessed: " << ex.what() << "\n";
    }

    // 5. Access Method B: std::get_if (Returns a pointer, or nullptr if wrong)
    if (auto pVal = std::get_if<std::string>(&myVariant)) {
        std::cout << "Safe pointer access: " << *pVal << "\n";
    } else {
        std::cout << "Not a string\n";
    }
}