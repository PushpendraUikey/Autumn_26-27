#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
using namespace std;

/*
Always think in terms of different order of thread execution to get the general idea of what is going to be 
the different sequence of execution and how the threads are going to interact with each other.
e.g. let the thread 1 run first and then thread 2, then thread 2 first and then thread 1, 
then let them run in parallel and see what happens. - Great way to understand the problem and the solution.
*/

mutex m;
condition_variable cv;

void f (int id) {
	unique_lock<mutex> l(m);
	if (id!=0) cv.wait(l);
	else {
		for (int i=0;i<4;i++){
			cout << i << endl;
			// this_thread::sleep_for(300ms);
		}
	}
	cv.notify_all();
};


int main () {

 thread t1(f,0);
 thread t2(f,1);
 this_thread::sleep_for(300ms);
 t1.join();
 t2.join();

}