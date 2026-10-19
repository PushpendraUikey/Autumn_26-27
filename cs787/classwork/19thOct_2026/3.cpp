#include <iostream>
#include <type_traits>
using namespace std;

class A {};

int main () {

int i;

bool b1 = is_class<int>::value;
bool b2 = is_class<string>::value;
bool b3 = is_class<float>::value;
bool b4 = is_class<A>::value;

cout << b1 << b2 << b3 << b4 << endl;


}

