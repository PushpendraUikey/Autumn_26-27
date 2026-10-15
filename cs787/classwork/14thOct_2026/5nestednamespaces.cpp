#include <iostream>
using namespace std;


namespace H {
	int x = 10;
}

namespace G {
	int y=20;
using namespace H;
}

int main() {
using namespace G;
using namespace H;
	cout << x << "\n" << y << "\n"; //will this work?
}
