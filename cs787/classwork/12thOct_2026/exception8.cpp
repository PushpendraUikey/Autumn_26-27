#include <iostream>
using namespace std;
class A {
	public:
	 A() { 
		try {throw 1;} 
		catch (...) {cout << "caught in constructor\n";} 
	}
	 void f() {cout << "makes sense?\n";} 
};

int main () {

 try {
	A a;
	cout <<"calling a.f\n";
	a.f();
 } catch (...) {cout <<"caught in main\n";}
}

// an exception can leave a constructor in C++, and throwing an exception is the standard, idiomatic way to signal 
// that object construction has failed. Because constructors do not have a return value, 
// hence we cannot use return codes to report an error.

// If an exception escapes the constructor, the destructor for that specific object will never be called.
