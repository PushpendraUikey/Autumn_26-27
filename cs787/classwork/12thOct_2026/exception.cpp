#include <iostream>
using namespace std;

class account {
int bal;
public :
	account (int a) {if (a<1000) throw ('n'); bal = a;}
	void deposit (int a) {bal+=a;}
	void withdraw (int a) {
		if (bal < 100) throw (bal);
		bal -= a;
	};
	int balance () {return bal;}
};

int main () {


try {
	account * a1 = new account(10000);
	a1->withdraw(200);
	cout << a1->balance() << endl;
	account * a2 = new account(10);
	a2->withdraw(200);
	cout << a1->balance() << endl;
}
catch (char c) {
	cout << "Exception occured, balance is low !\n";
}
/*catch (char c) {
	cout << "Exception occured, balance is low\n";
} */ // uncomment this to read the error (warning message) 
catch (int b) {
	cout << "Exception occured, balance is just Rs.  " << b << "\n";
};


}


// catching an exception occur is done using the "type of exception(data type)" being thrown.