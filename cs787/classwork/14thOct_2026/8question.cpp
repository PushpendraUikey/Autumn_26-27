#include <iostream>
using namespace std;

namespace H {
 int x = 10;
 int y=4;
}
namespace G {
  int y=20;
  using namespace H;
  void f(void) {cout<<y<<"....\n";}
}

void g() { using namespace G; }
int main() {
 int y = 25;
 using namespace G;
// g();  // uncomment this, and comment the previous? 
 cout << x << "\n" << y << "\n"; //will this work?
 f();
}

// using inside a function block only brings the names into scope for that function.
// It does not affect caller or callee or global scope. It is mere local scope.