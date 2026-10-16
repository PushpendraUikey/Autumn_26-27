#include <iostream>
#include <string>

int main() {
  std::cout << 10 << std::endl;  // Member Scope Lookup (MSL) finds operator << in std namespace.
  std::string str = "hello world";
  std::cout << str; // operator << is found correctly due to ADL
}
