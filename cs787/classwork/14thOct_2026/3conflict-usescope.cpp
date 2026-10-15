#include <iostream>
using namespace std;


namespace H {
	int x = 10;
}


int main() {
int x = 20;
using namespace H;
	cout << x << "\n"; //which x?

	int &x = H::x;
	cout << x << "\n"; //which x?

}
