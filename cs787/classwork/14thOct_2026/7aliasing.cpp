#include <iostream>
using namespace std;

namespace H {
	int x = 10;
	int y = 10;
}
namespace A = H;
namespace B = A;
namespace G {
	int y=20;
}
using namespace B;

//using namespace H;
int main() {
	cout << x << "\n" << y << "\n"; //will this work?
}
