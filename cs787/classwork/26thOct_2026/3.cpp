#include <iostream>

using namespace std;

class A {
int p; 
public:
	A(int x) {p=x;}
	A(const A& a, int v) {cout << "in copy constructor\n";p=a.p*v;}
		//  you can have additional argument, the first one is must
	void dump() {cout << p << endl;}
};


int main () {

A a(10);
A c(a,1000); // copy constructor is called
c.dump();
A d(a); 	// Why is defualt copy constructor still available?
d.dump();
A e(a,100);
// A f; //Compiler error: Since we've defined the constructor for class A, the default constructor is no longer available.
}
