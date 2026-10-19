#include <iostream>
using namespace std;

// no types needed in lambda expressions
// it works polymorphic

class C  {
 public: 
	int v;
	C(int i) { v = i;} 
};

int operator + (C x, C y) {
	return (x.v * y.v);
};

auto f = [] (auto x, auto y) {
	return x+y;
}; 


int main () {

C c1(20), c2(30);
cout << f(20, 30) << endl;
cout << f(c1, c2) << endl;

}
