#include <cassert>
class account {
int bal;
public :
	account (int a) {bal = a;}
	void deposit (int a) {bal+=a;}
	void withdraw (int a) {
		assert (bal > 100);
		bal -= a;
	};
	int balance () {return bal;}
};

int main () {

account * a = new account(10);

	a->withdraw(200);


}


