#include <iostream>
using namespace std;
template<typename... Args>
auto sum1(Args... args) {
    return (... + args); // Unary left fold: sums all arguments cleanly
    //((a + b) + c)
}
template<typename... Args>
auto sum2(Args... args) {
    return (args + ... ); // Unary right fold: sums all arguments cleanly
    //(a + (b + c))
}

// fold expression: no recursion at all
template <typename... Args>
void print(const Args&... args) {
    ((cout << args << " "), ...);   // ... tells the compiler to copy the pattern
    // cout << args << " " once for each element of args, joined by commas.
    cout << "last\n";
}

int main () {

 cout << sum1 (10,20,30) << endl;
 cout << sum1 (10,20) << endl;
 cout << sum1 (string("hello "), string("world "), string("how "), string("are "),
string("you!")) << endl;    // In this example, plain literals won't compile, because
    // there's no '+' operation for the type 'const char*'.
    // but std::string has

 cout << sum2 (10,20,30) << endl;
 cout << sum2 (10,20) << endl;

 ((cout << 10 << " "), 10); // right side 10 is evaluated and thrown away.
 // nothing tells the compiler to print it.
 print(1,2,3,5,3);
}

/*
A fold expression applies a binary operator to every element of a parameter pack in one expression.
You don't need recursion or a base case. The compiler expands it at compile time. So sum1(10,20,30)
expands as you had written 10 + 20 + 30.

Values are not guaranteed to be evaluated at runtime, most of them are evaluated at runtime, it is
just that they are unfolded into expression at the compile time. 
Sometimes the compiler optimization does the evaluation does the evaluation at compile time itself
which is not guaranteed.

1. An empty pack fails to compile.
2. A binary fold fixes that. Give it a starting value, called 
init: (0 + ... + args) - binary left fold or (args + ... + 0)
binary right fold. init should make sense for the type. 0 + string
won't compile.

3. auto return type: The return type is whatever type the folded 
expression produces.

4. The comma fold is the common trick for printing. 
((cout << args << " "), ...); is how the print can be written
without recursion.
*/