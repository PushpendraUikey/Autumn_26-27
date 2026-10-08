#include <iostream>
using namespace std;

template <int N>
struct Fib {
	enum { val = Fib<N-1>::val + Fib<N-2>::val };
};
template <>
struct Fib<0> {
	enum {val=1};
};
template <>
struct Fib<1> {
    enum {val=1};
};

int main() {

    cout << Fib<5>::val << endl;
    cout << Fib<0>::val << endl;
    cout << Fib<2>::val << endl;
    cout << Fib<3>::val << endl;
}