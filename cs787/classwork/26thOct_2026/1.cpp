#include <iostream>

using namespace std;

class A {
int p; 
public:
	A(int x) {p=x;}
	A(const A& a) {cout << "in copy constructor\n";p=a.p*a.p;}
		// constructor called when object is constructed!
	void dump() {cout << p << endl;}
};


int main () {

A a(10);
A b(20);
a = b; // copy constructor is not called 
a.dump();

A c(a); // copy constructor is called
c.dump();

}
