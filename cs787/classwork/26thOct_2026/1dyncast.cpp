#include <iostream>
using namespace std;

class A {
protected:
int a;
public:
	A(int e) {a=e;}
	virtual void f(){}
};

class B : public A {
int b;
public:
	B(int e, int f):A(e){b=f;}
	virtual void f(){}
	void dump() {cout << a << " " << b << endl;}

};

int main () {

A *ap;
B *bp = new B(10,20);

// B *bp = ap;  // does not compile

 ap = bp;

B *bp2;

// bp2 = ap;  // does not compile
 bp2 = dynamic_cast<B *> (ap);  // goes through

 if(bp2 != NULL)
 	bp2->dump();
}

/*
Base class pointer can point to the derived class object but not vice versa.
Hence dynamic_cast is one way to convert base class pointer to derived class pointer.
If the base class pointer is not pointing to the derived class object then dynamic_cast will return NULL.
*/