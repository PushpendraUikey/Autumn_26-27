#include <iostream>
using namespace std;
namespace A {
	class X {};
	void f() {cout << "hello\n";};
	void g()  {cout << "hi\n";};
}
namespace B{
	class Y {};
	void f(Y y) {cout << "greetings\n";};
	void g(Y y) {cout << "namashkar\n";};
}

int main () {

A::X x;
B::Y y;
A::f();
A::g();
f(y);
g(y);
}

/*
Qualified Calls: A::f() and A::g()
Unqualified Calls: f(y) and g(y). But it still doesn't result in Compiler Error ADL (Argument Dependent Lookup)
	is used here. 
	When compiler sees an unqualified function call and it doesn't find it in the current scope, 

	It examines the argument passed to the function. It sees the namespace attached to the argument and looks for
	the function in that namespace. If it finds the function, it calls it.
*/