#include <iostream>
#include <vector>
using namespace std;

class A {
int x;
 public:
	A(int e) {x=e; cout << "in constructor:" << e << "\n";}
	A(const A &a) {cout << "copy constructor called " << a.x << endl; x = a.x*100;}
	void f() {cout << x << endl;}

};

int main () {

	A a1(10), a2(20), a3(30);
	A avec[3]={a1,a2,a3};
	for (auto &x : avec) {
		x.f() ;
	}

}
