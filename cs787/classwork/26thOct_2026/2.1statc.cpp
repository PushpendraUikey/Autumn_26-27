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

class C : public A {
int c;
public:
	C(int e, int f):A(e){c=f;}
	void dump() {cout << a << " " << c << endl;};
};

int main () {


A *ap = new B(10,20);


int choice;

	cin >> choice;
	if (choice==0) ap = new B(10,20);
	else ap = new C(30,40);
 
	C *cp = static_cast<C *> (ap); // reports error

	if (cp==NULL) cout << "got nullpointer" << endl;
	cp->dump();

}

/*
Use of static_cast is left to the expertise of the programmer. It is the programmer's responsibility to ensure 
that the base class pointer is pointing to the derived class object. If not, then the program first tries to 
force converting the base class pointer to derived class pointer and if that is not possible then it may 
lead to undefined behavior or even crash.
*/
