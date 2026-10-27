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
 
	C *cp = dynamic_cast<C *> (ap); // reports error

	if (cp==NULL) cout << "got nullpointer" << endl;
	else cp->dump();

}

