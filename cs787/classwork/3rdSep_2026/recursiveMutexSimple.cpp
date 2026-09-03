#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

/*
Following example keeps the lock exclusive to the thread which has acquired it first, so that thread will do all the job 
and other thread will be blocked until first thread completely finishes its execution and releases the lock.
*/

recursive_mutex m;

int counter = 11;

void f(string msg) {
    m.lock();
    if(counter <= 0) {
        m.unlock();
        return;
    }
    cout << msg << " with value " << counter << endl;
    counter--;
    //m.unlock(); // unlock here would allow the other thread to acquire the lock and do some work
        // which we want to avoid.
    this_thread::sleep_for(chrono::microseconds(300));
    f(msg);
    m.unlock(); // needed else deadlock (Even though single thread will do 
            // all job but wouldn't be able to unlock automatically since it is not using unique_lock/lock_guard)
}

int main() {
    thread t1(f, "T1");
    thread t2(f, "T2");
    t1.join();
    t2.join();
}