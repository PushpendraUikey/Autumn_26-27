#include <iostream>
using namespace std;

/*
"Zombie" object (Silent semantic failure) - Show the danger of imporperly swallowing exceptions inside a
constructor. Created a "zombie" object that compiler thinks is valid, but is not.

Since the throw is swallowed inside the constructor, the object is created. a->i is valid but a->j is not valid.
Later if programmer tries to access a->j, it will lead to undefined behavior.

Critical Lesson: If a constructor can't fully initialize all class invariant, it must let the exception escape
to alert the caller that the object is invalid.

Need ==> We must have the way to tell the programmer that the object constructor has failed.
*/
class A {
	int i;
	A* j;
	public:
	 A() { try {i = 1; throw 1; j=new A();} catch (...) {cout <<"error\n";}}
	 void f() {cout << i << " makes sense?\n";} 
};

int main () {

A *a;
	a = new A();
	cout <<"calling a.f\n";
	a->f();
}
