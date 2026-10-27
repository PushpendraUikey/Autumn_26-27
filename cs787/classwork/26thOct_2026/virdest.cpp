#include <iostream>
using namespace std;

// making the destructor virtual when inheritance is possible makes each of the class's destructor get called
// in the base-child class hierarchy, hence correctly removing any of the memory footprint that might 
// not have removed without it.

class A {
public:
	virtual ~A() {cout << "destroying A obj\n";}
};
class B: public A {
public:
	virtual ~B() {cout << "destroying B obj\n";}
};


int main () {

 A *ap = new B();

 delete (ap);

}
