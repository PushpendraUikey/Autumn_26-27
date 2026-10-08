#include <iostream>
#include <stack>
using namespace std;

template <typename T>
class A {
	stack <T> s;
	public:
		A() { cout << "A constructor" << endl; }
		~A() { cout << "A destructor" << endl; }

		A(const A &a) {
			cout << "A copy constructor" << endl;
			s = a.s;
		}
		A(A &&a) {
			cout << "A move constructor" << endl;
			s = std::move(a.s);
		}
};


int main () {

	stack <int> s;
	int e;
	s.push(10);
	s.push(20);
	s.push(30);

	while (!s.empty()) {
		cout << "  stack size:" << s.size() << endl;
		e = s.top();
		cout << e << endl;
		s.pop();
	}
	s.pop();// it'll thow an exception because stack is empty now.
};

