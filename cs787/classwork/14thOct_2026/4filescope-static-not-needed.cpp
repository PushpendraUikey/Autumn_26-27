#include <iostream>
using namespace std;
void h();
void f(void);
static int x=100;
//can be rewritten as:
// namespace { int x;}
namespace {int y=10;}
int z=1000;
void f(void) { 
	x++; y=x+1; z++; 
	cout << x << " " << y << " " << z << "\n";
}
void g() {
	y++;
}
int main () {
 f();g(); h();
 cout << x << "  " << y << " " << z << endl;
}
