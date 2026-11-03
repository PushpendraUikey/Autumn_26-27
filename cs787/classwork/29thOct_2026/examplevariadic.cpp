#include <iostream>
using namespace std;

// Need to provide a base case for the recursion to terminate
void print () { cout << "last\n";} // try commenting this

// Variadic template function to handle multiple arguments
template <typename T, typename... VARS> // typename... VARS is a template parameter pack: zero or more types
void print (T e, VARS... el) {  // VARS... el is the matching function parameter pack: zero or more values.
  cout << e << " ";
  print (el...);  // pack expansion: The pack is unpacked back into a comma-separated list of arguments.
}

int main () {

print (1,2,3,5,3);
print (1,2);
print (1,2,3,5,"hello", 10.123);
print (1,2,3,3);

}

/*
The recursion happens at compile time. The compiler doesn't create
one function that calls itself, but instead it creates a separate 
function for each argument list.

print<int,int,int,int,int>(1,2,3,5,3)  → prints 1, calls print(2,3,5,3)
print<int,int,int,int>(2,3,5,3)        → prints 2, calls print(3,5,3)
print<int,int,int>(3,5,3)              → prints 3, calls print(5,3)
print<int,int>(5,3)                    → prints 5, calls print(3)
print<int>(3)                          → prints 3, calls print()   ← VARS is empty
print()                                → non-template base case, prints "last"

*/