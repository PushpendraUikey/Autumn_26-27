#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

long counter = 0;
mutex mtx;
std::condition_variable cv;

void f() {
    while(true) {
        {
            // holding the lock only for the required condition to be met
            unique_lock<mutex> ul(mtx);
            cv.wait(ul, []{ return counter%4 != 0; });
        }
        
        cout  << "Message1 : Before\n";
        this_thread::sleep_for(100ms);
        cout << "Message2 : After\n";

    }
}

void g() {
    while(true) {
        bool should_notify_f = false;
        {
            lock_guard<mutex> lg(mtx);
            counter++;

            if(counter % 4 == 1) should_notify_f = true;
        }
        if(should_notify_f) cv.notify_one();

        this_thread::sleep_for(300ms);
        std::cout << "Counter value from g: " << counter << endl; // this is fine since counter is being read onlyand no other can write to it
    }
}

int main() {
    thread t1(f);
    thread t2(g);

    t1.join();
    t2.join();

    return 0;
}