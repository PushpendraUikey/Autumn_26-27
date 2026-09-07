#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
using namespace std;


mutex m;
condition_variable cv;
bool ready = true;

void f () {
unique_lock<mutex> ul(m);
	while (1) {
		if(!ready) {
			ready = true;
			cv.wait(ul);
/*
This program correctly runs however with one underlying subtle problem that is cv.notify_one()
is called at the end is before unlocking the lock so it dosn't wake up the other thread from the sleep state
to run rather since the lock is still held by the current thread it'll be just pushed to mutex queue.

kind of inefficiency to waking from cv queue just to be kept in teh mutex queue.
*/
		}
		ready = false;
		for (int i=0; i< 4; i++) {
			cout << this_thread::get_id() << " " << i << endl;
			this_thread::sleep_for(300ms);
		}	
		cv.notify_one();
	}
};


int main () {

 thread t1(f);
 thread t2(f);
 this_thread::sleep_for(100ms);
 cv.notify_one();
 t1.join();
 t2.join();

}

/*
This might not always work if the main thread still somehow executes first and cv.notify_one() is called
before the other threads are waiting on the condition variable.
*/

