#include <iostream>
using namespace std;
namespace B { class Y; }
namespace A {
	class X {};
	class Y {};
	void f(Y y) {cout << "welcome\n";};
	void g(X x, Y y)  {cout << "blessings\n";};
	void f(X x) {cout << "hello\n"; Y y; f(y);};
	void g(X x)  {cout << "hi\n";};
	void mi(B::Y y, X x);
}
namespace B{
	class Y {};
	void f(Y y) {cout << "greetings\n";};
	void g(Y y) {cout << "namashkar\n";};
	void mip(A::Y y) {cout << "namaste\n";};
}
void A::mi(B::Y y, X x) {cout << "namaste\n";}
int main () {
A::X x;
B::Y y;
A::Y y2;
f(x);
g(x);
f(y);
g(y);
g(x,y);
mi(y,x);
mip(y2);
}

/*
There's no ordering between the arguments to lookup for the function name in their respective namespaces.
When multiple namespaces contain a function with the same name, the compiler gathers all matching functions 
into a single pool of candidate functions and applies standard C++ Overload Resolution rules. 
	Either Ambiguity or
	a Single Best Match is found.
*/