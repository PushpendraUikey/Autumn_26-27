#include <iostream>
#include <type_traits>
using namespace std;

class A {};

int main () {

int i;

bool b1 = is_integral<int>::value;
bool b2 = is_integral<string>::value;
bool b3 = is_integral<float>::value;
bool b4 = is_integral<A>::value;

cout << b1 << b2 << b3 << b4 << endl;


}

