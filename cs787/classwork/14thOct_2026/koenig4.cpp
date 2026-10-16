#include <iostream>
using namespace std;
namespace A {
	class X {};
	class Y {};
	void f(Y y) {cout << "welcome\n";};
	void g(X x,Y y)  {cout << "blessings\n";};
	void f(X x) {cout << "hello\n"; Y y; f(y);};
	void g(X x)  {cout << "hi\n";};
}
namespace B{
	class Y {};
	void f(Y y) {cout << "greetings\n";};
	void g(Y y) {cout << "namashkar\n";};
}

int main () {

A::X x;
A::Y y;
f(x);
g(x);
f(y);
g(y);
g(x,y);
}
