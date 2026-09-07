#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
using namespace std;


mutex m;
condition_variable cv;
atomic<bool> ready{true};
void f () {
unique_lock<mutex> ul(m);
	while (1) {
		cv.wait(ul);
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


