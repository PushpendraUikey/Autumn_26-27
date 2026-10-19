#include <iostream>
using namespace std;

// Defines a type named 'MathOperation' for a function 
// that takes two ints and returns an int
using MathOperation = int(*)(int, int);   // defines a type alias 

// Usage:
int add(int a, int b) { return a + b; }

int main () {
MathOperation op = add; // 'op' is now a variable of type 'MathOperation'
int result = op(5, 3);  // Call it directly

cout << result << endl;

}

