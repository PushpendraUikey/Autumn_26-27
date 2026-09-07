#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
using namespace std;

/*
The "Rendezvous" Pattern
A "rendezvous" in concurrency is a synchronization point where multiple threads must meet before any of them can proceed. 
In your code, you accidentally created a cascading rendezvous (often called a relay or baton-passing pattern) using 
condition variables.
*/
mutex m;
condition_variable cv;

int x = 0;

void f (int id) {
	unique_lock<mutex> l(m);
	cv.wait(l);
	for (int i=0;i<4;i++){
			cout << this_thread::get_id() << " " << x++ << endl;
			this_thread::sleep_for(300ms);
		}
	cv.notify_all();
};


int main () {

 thread t1(f,0);
 thread t2(f,1);
 thread t3(f,2);
 this_thread::sleep_for(300ms);
 cv.notify_one();
 t1.join();
 t2.join();
 t3.join();

}


