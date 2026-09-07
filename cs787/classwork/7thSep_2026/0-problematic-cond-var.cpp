#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
using namespace std;

/*
In this example, we have two threads both trying to acquire the same lock and wait on a condition variable, without
any mechanism to unblock the waiting thread. <- This is deadlock. 
The first thread acquire the lock and goes to wait on the condition variable, but the second thread is also trying to 
acquire the same lock and will be blocked.
*/
mutex m;
condition_variable cv;
atomic<bool> should_proceed{true};

void f () {
	while (1) {
		unique_lock<mutex> ul(m);

		if(!should_proceed.load()) cv.wait(ul);
		should_proceed = false;
		for (int i=0; i< 4; i++) {
			cout << this_thread::get_id() << " " << i << endl;
			this_thread::sleep_for(300ms);
		}
		should_proceed = true;
		ul.unlock();
		cv.notify_one();
		this_thread::sleep_for(300ms);
	}
};


int main () {

 thread t1(f);
 thread t2(f);
 t1.join();
 t2.join();

}