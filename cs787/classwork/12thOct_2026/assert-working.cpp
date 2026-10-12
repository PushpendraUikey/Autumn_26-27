#include<iostream>
#include <cassert>
using namespace std;

#define NDEBUG 

class account {
int bal;
public :
	account (int a) {bal = a;}
	void deposit (int a) {bal+=a;}
	void withdraw (int a) {
		#ifndef NDEBUG
    		if (bal <100) {
        		std::fprintf(stderr, "Assertion failed: bal < 100, line 13\n");
			cerr << "ERROR\n";
        		std::abort();
    		}
		#endif
		bal -= a;
	};
	int balance () {return bal;}
};

int main () {

account * a = new account(10);

	a->withdraw(200);
	cout << "balance is:" <<  a->balance() << endl;


}


