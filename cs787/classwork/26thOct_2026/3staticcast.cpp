#include <iostream>
using namespace std;

class A {
protected:
int a;
public:
	A(int e) {a=e;}
};

class B : public A {
int k=111;
int b;	// Try swapping the order of declaration of c and b and see the effect on the output.
public:
	B(int e, int f):A(e){b=f;}
	void f() { cout << "Hello\n"; }
	void dump() {cout << a << " " << b << endl;}
};

class C : public A {
int c;
public:
	C(int e, int f):A(e){c=f;}
	void dump() {cout << a << " " << c << endl;} // if you defined this as virtual then segfault will occur. Bcz Runtime resolution will be used
};

int main () {


A *ap = new B(10,20);


int choice;

	cin >> choice;
	if (choice==0) ap = new B(10,20);
	else ap = new C(30,40);
 
	C *cp = static_cast<C *> (ap); // may go ahead erroneously, since forced

	cp->dump();

}

// lot of confusion in this program.
/*
static function resolution (early binding) and object memory layout.

Because we used static_cast, the compiler was forced to blindly treat the memory address as if it holds a 'C'
object.

Why it works normally for choice != 0:
    ap is assigned a new C object,
	The static_cast<*C>(ap) is perfectly valid because the underlying object is actually a C object. 
	cp->dump() correctly accesses C's memory.

Confusion when choice == 0:
    ap is assigned a new B(10,20) object,
	The pointer cp is forced to point to B but compiler thinks it is pointing to C object.

	1. Why does it call dump() when B doesn't have a dump() method?
	   In C++, unless a method is marked virtual, function calls are resolved at compile time based strictly
	   on the type of the pointer, not the actual object it points to. Because cp is declared as a C*, the compiler 
	   statically wires cp->dump() directly to the C::dump() method. Doesn't matter if the actual object in 
	   memory is a B object.

	2. Why does it print 10 0?
		When C::dump() executes, it needs to print a and c. It does this by looking at specific memory offsets 
		relative to the pointer's address. 
			Here's the memory layout of object B:
			Offset 0: A::a (10)
			Offset 4: B::k (Value uninitialized, which happened to be 0 in this run)
			Offset 8: B::b (20)
		    What C::dump() expects to see (Layout of C):
			Offset 0: A::a
			Offset 4: C::c
		When cp->dump() is called, it accesses the memory at offset 0 for a (which is B::a) and 
		offset 4 for c (which is actually B::k)

NOTE: Accessing an object through a pointer of an incorrect type like this is formally classified as 
    "Undefined Behavior" in C++, meaning compiler makes no guarantee about what will happen.
*/