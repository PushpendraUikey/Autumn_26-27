#include <iostream>
#include <vector>
using namespace std;

int main () {
	string s("hello");
	cout << "size: " << s.size() << endl;
	cout << "capacity: " << s.capacity() << endl;
	for (auto &x : s) {
		cout << x << endl;
	}
}
