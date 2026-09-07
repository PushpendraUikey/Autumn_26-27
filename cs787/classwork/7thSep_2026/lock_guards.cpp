#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

class A {

 mutex m;
 public:
	void f() {
		lock_guard<mutex> lg(m);
		for (int i =0; i<4; i++){
			cout << "----" << i << endl;
			//lg.unlock(); // not allowed on lock_guards
			//lg.lock(); // not allowed on lock_guards
			this_thread::sleep_for(400ms);
		}

	}

	void g() {
		lock_guard<mutex> lg(m);
		for (int i =0; i<4; i++) {
			cout << "++++" << i << endl;
			this_thread::sleep_for(400ms);
		}

	}
};

int main (){

 A *a = new A();
	thread t1(&A::f,a);
	thread t2(&A::g,a);

	t1.join();
	t2.join();
}
