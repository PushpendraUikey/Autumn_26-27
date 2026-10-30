#include <iostream>
using namespace std;

class account {
	int bal;
	public :
		account (int a) {bal = a;}
		void deposit (int a) {bal+=a;}
		void withdraw (int a) {
			if (bal < 100) throw (bal);
			bal -= a;
		};
		int balance () {return bal;}
};

int main () {
	account * a = new account(10);
	try {
		a->withdraw(200);
	}
	catch (char b) {
		cout << "Exception occured, balance is just Rs.  " << b << "\n";
	}
	catch (...) {
		cout << "Default Exception handler\n";
	}
}


