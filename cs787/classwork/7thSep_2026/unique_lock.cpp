#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

//RAAI - Resource Acquisition is initialization (auto lock and
//auto release when it goes out of scope

// lock unlock allowed, but at run time if deadlock possibility comes in
// due to self relocking attempt, it has undefined behavior


class A {

 mutex m;
 public:
	void f() {
		unique_lock <mutex> ul(m);
		for (int i =0; i<4; i++){
			cout << "----" << i << endl;
			if (i%2) ul.unlock(); // allowed on unique locks if safe
			this_thread::sleep_for(400ms);
			if (i%2) ul.lock(); // allowed on unique locks if safe 
			this_thread::sleep_for(400ms);
		}

	}

	void g() {
		unique_lock <mutex> ul(m);
		for (int i =0; i<4; i++){
			cout << "++++" << i << endl;
			if (i%2) ul.unlock(); // allowed on lock_guards
			this_thread::sleep_for(400ms);
			if (i%2) ul.lock(); // allowed on lock_guards
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
