#include <iostream>
using namespace std;

// Number of terms in the Collatz sequence of N (N, ..., 1), computed at compile time.
// even: N -> N/2, odd: N -> 3N+1

template <int N> struct Steps;

// Helper picks the next term based on a bool template parameter, so only the
// branch that is actually taken gets instantiated.
template <int N, bool isEven>
struct NextStep;

template <int N>
struct NextStep<N, true> {
	enum { val = Steps<N/2>::val + 1 };
};
template <int N>
struct NextStep<N, false> {
	enum { val = Steps<3*N+1>::val + 1 };
};

template <int N>
struct Steps {
	enum { val = NextStep<N, N%2==0>::val };
};
template <>
struct Steps<1> {
	enum {val=1};
};

int main() {

    cout << Steps<5>::val << endl;
    cout << Steps<1>::val << endl;
    cout << Steps<2>::val << endl;
    cout << Steps<3>::val << endl;
    cout << Steps<4>::val << endl;
    cout << Steps<6>::val << endl;
}

/*
'if' cannot be used inside a struct body, so the even/odd decision is moved into the
template arguments: NextStep<N, N%2==0> selects one of two partial specializations.

A ternary inside the enum, i.e. N%2==0 ? Steps<N/2>::val : Steps<3*N+1>::val, does not work:
the compiler instantiates BOTH Steps<N/2> and Steps<3*N+1>. The untaken branches fan out and
eventually lead back to a Steps<> still being defined (e.g. Steps<5> -> ... -> Steps<10> ->
Steps<5>), giving "incomplete type 'Steps<5>' used in nested name specifier", or they keep
growing until the template depth limit is exceeded.

Steps<5>: 5 -> 16 -> 8 -> 4 -> 2 -> 1   => 6
*/
