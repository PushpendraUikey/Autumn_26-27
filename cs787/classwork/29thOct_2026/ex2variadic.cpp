#include <iostream>
using namespace std;


void print () { cout << "last\n";}

template <typename T, typename... VARS>
void print (T e, VARS... el) {
  cout << e << endl;
  print (el...);
}

class A{ 
	public:
 	int p;
	A(int e){p=e;}
};

ostream& operator << (ostream &os, const A a) {os << a.p;return os;}

int main () {


A a(1000);

print (1,2,3,5,3,"hello",string("how are you"),a);

}
