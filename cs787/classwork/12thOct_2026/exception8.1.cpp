#include <iostream>
using namespace std;

/*
Intent of the program is to highlight a severe danger involving dynamic memory(new).
*/
class A {
	int i;
	public:
	 A() {i = 1; throw 1;}
	 void f() {cout << i << " makes sense?\n";} 
	 ~A() {cout << "destructor\n";}	// destructor will not be called if constructor throws an exception
};

int main () {

A *a;
 try {
	a = new A(); // Because the constructor failed, C++ runtime safely deallocates the memory it just
				// reserved for the object, and the pointer a is left dangling. (Assignment never happens)
  }
  catch (...){cout << "caught\n";}
cout <<"calling a.f\n";
a->f();	// Since static resolution will happen at compile time (method is not virtual), it'll call void f()
  		// of class A. which will try to access i, at some offset of base address => Will get a garbage.
}
