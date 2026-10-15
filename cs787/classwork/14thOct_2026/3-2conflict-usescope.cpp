#include <iostream>
using namespace std;


namespace H {
	int x = 10;
}

namespace K {
	int x = 30;
}

namespace G {
	int y = 100;
}

int main() {
// int x = 20;
using namespace H;
// using namespace K;
	cout << x << "\n"; //which x?

	int &y = H::x;
	cout << x << "\n"; //which x?

}
