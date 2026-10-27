#include <iostream>

using namespace std;

class A {
int p; 
public:
	A(int x) {p=x;}
	A(const A& a) {cout << "in copy constructor\n";p=a.p*a.p;}
	A & operator = (const A &a) {p=a.p+20; return *this;}
	// assignment operator overloaded 
	void dump() {cout << p << endl;}
};


int main () {

A a(10);
A c(a); // default copy constructor is called
c.dump();
c = a;
c.dump();

}
