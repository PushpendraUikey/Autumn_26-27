#include <iostream>
using namespace std;
class A {
	public:
	 	A() try { throw 1;	} 
			catch (...) {cout << "caught inside constructor\n";} 
	 	void f() {cout << "makes sense?\n";} 
		~A() {cout << "destructor called\n";}
};

int main () {
 try {
	A a;
	cout << "calling a.f\n";
	a.f();
 } catch (...) { cout << "In main here..\n";};
}

// If a handler of a constructor function-try-block finishes normally, the currently handled exception is 
// automatically rethrown. The handler cannot successfully recover from the failed construction.

// The main reason for introducing function-try-blocks in C++ was to handle exceptions thrown during constructor 
// initialization, particularly exceptions from base classes and data members, which an ordinary try block inside 
// the constructor body cannot catch.

// if an exception is thrown from a constructor, the destructor for that specific object will never be called.
// also, to tell the caller that the constructor has failed, the exception must be propagated out of the constructor.
// which is done using a function-try-block. If the handler of a function-try-block finishes normally, the currently 
// handled exception is automatically rethrown. The handler cannot successfully recover from the failed construction.