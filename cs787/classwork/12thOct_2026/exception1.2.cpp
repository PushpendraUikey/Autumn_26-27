#include <iostream>
using namespace std;

class Account {
int bal;
public :
	Account (int a) {bal = a;}
	void deposit (int a) noexcept; // does not throw exception
	void withdraw (int a) ; 
	int balance () noexcept { throw(-1); return bal;} // does not throw exception
};
void Account::deposit (int a) noexcept {bal+=a; if (a<10) throw 0;}
void Account::withdraw (int a) { 
	if (bal < 100) throw (bal);
	bal -= a;
};

int main () {

Account * a = new Account(10);

try {
	a->deposit(5);
	a->withdraw(200);
}
catch (int b) { cout << "exception thrown from noexcept function\n";}
try {
	a->withdraw(200);
}
catch (int b) {
	cout << "Exception occured, balance is just Rs.  " << b << "\n";
};




}


// noexcept is a specifier that hints the compiler that this method would not throw any exception.
// Yet if you declare a throw() inside the method, it will warn at compile time and then if the 
// exception is thrown, it will terminate the program. 
// Try-Catch block will not work in this case. They'll be discarded if it is noexcept.