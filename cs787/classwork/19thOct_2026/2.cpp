#include <iostream>
#include <type_traits>
using namespace std;

class A {};

int main () {

int i;

bool b1 = is_floating_point<int>::value;
bool b2 = is_floating_point<string>::value;
bool b3 = is_floating_point<float>::value;
bool b4 = is_floating_point<A>::value;

cout << b1 << b2 << b3 << b4 << endl;


}

