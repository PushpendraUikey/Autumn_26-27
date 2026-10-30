#include <iostream>
using namespace std;

class A {
	int i;
	public:
	 	A() try {
			i=0;
			// problem occurs here
		 	throw 1;
		} catch (...) {cout << "caught inside constructor\n";} 
	 	
		void f() {cout << "makes sense?\n";} 
};

int main () {
	try {
		A a;
		cout <<"calling a.f\n";
		a.f();
	} catch (...) {cout <<"caught here in main\n";}
}
