#define NDEBUG	// This switches off the defensive programming and any assertion is removed during the compile time.
				// Without it all the assertions would be placed at compile time and checked during the runtime.
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


