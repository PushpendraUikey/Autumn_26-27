#include <iostream>
using namespace std;
namespace A {
	class X {};
	void f(X) {cout << "hello\n";};
}

int main () {

A::X x;
f(x);

}
