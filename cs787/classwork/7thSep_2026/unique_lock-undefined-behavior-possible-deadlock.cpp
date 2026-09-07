#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

//RAAI - Resource Acquisition is initialization (auto lock and
//auto release when it goes out of scope

// undefined behavior when a thread tries to lock if it
// already holds a lock-- may think it may cause a deadlock
// program may terminate -- check what your compiler does!

class A {

 mutex m;
 public:
	void f() {
		unique_lock <mutex> ul(m);
		for (int i =0; i<4; i++){
			cout << "----" << i << endl;
			if (i%2) 
			 ul.unlock(); // allowed on unique lock            
			this_thread::sleep_for(400ms);
			ul.lock(); // allowed on unique lock, may have undefined behavior
			this_thread::sleep_for(400ms);
		}

	}

	void g() {
		unique_lock <mutex> ul(m);
		for (int i =0; i<4; i++){
			cout << "++++" << i << endl;
			if (i%2) 
				ul.unlock(); // allowed on unique locks
			this_thread::sleep_for(400ms);
			if (i%2) 
				ul.lock(); // allowed on unique locks, may have undefined beh.
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
