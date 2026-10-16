#include <iostream>
using namespace std;
namespace NS {

  class A {};

  void f(A* a) { cout << "f\n";}

}  


int main() {
   NS::A a;
   f(&a);  // f found due to Argument depepdent lookup (ADL). 
}
