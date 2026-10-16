#include <iostream>
using namespace std;

namespace H {
	int x = 10;
	int y = 10;
}
namespace G {
	int y=20;
	using namespace H;
}
int main() {
using namespace G;
	cout << x << "\n" << y << "\n"; //will this work?
}

/*
The current program isn't an example of nested namespaces. For a namespace to be nested, it must be 
physically defined within another namespace. 

Above condition compiler brings the G::y and H::y into the same scope, which results in ambiguity when 
trying to access y. The compiler cannot determine which y to use, leading to a compilation error.
*/