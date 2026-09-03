#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

/*
try_lock() on a mutex attempts to acquire the lock without blocking. If the lock is already held by another thread, 
try_lock() returns false immediately, allowing the thread to perform other tasks or retry later. This can help prevent 
deadlocks in scenarios where multiple threads are competing for the same resources.
*/

mutex m;

void f() {
    while(!m.try_lock()) {
        cout << "f() is waiting for lock" << endl;
        this_thread::sleep_for(chrono::seconds(3));
    }
    cout << "f() has acquired the lock" << endl;
    m.unlock();
    this_thread::sleep_for(chrono::seconds(3));
}

void g() {
    while(!m.try_lock()) {
        cout << "g() is waiting for lock" << endl;
        this_thread::sleep_for(chrono::seconds(3));
    }
    cout << "g() has acquired the lock" << endl;
    m.unlock();
    this_thread::sleep_for(chrono::seconds(3));
}

int main() {
    thread t1(f);
    thread t2(g);
    t1.join();
    t2.join();
}