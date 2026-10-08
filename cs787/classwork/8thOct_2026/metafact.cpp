#include <iostream>
using namespace std;

template <int N>
struct Fact {
	enum { val = N*Fact<N-1>::val};
};
template <>
struct Fact<0> {
	enum {val=1};
};

int main () {
 cout << Fact<5>::val << endl;
 cout << Fact<0>::val << endl;
 cout << Fact<2>::val << endl;
}

/*
Compiler is computing the factorial at compile time. The compiler is able to compute the factorial of a number at 
compile time because it uses template metaprogramming. The template metaprogramming technique allows the compiler
to generate code based on templates, which can be used to perform computations at compile time. 
*/