#include<bits/stdc++.h>
#include <thread>
using namespace std;

class A {
    mutex m;
    public: 
        void f() {
            std::cout << "f() is starting" << std::endl;
            lock_guard<mutex> lg(m);
            for (int i=0; i<4; i++) {
                cout << " .... " << i << endl;
                // if(i==2) {
                //     lg.unlock(); // not allowed: Throws error.
                // }
                // lg.lock()  // not allowed
            }
            this_thread::sleep_for(chrono::microseconds(300));
            std::cout << "f() is done" << std::endl;
        }
        void g() {
            std::cout << "g() is starting" << std::endl;
            lock_guard<mutex> lg(m);
            for(int i=0; i<4; i++) {
                cout << " ++++ " << i << endl;
            }
            this_thread::sleep_for(chrono::microseconds(300));
            std::cout << "g() is done" << std::endl;
        }
};

int main() {
    A *a = new A();
    thread t1(&A::f, a);
    thread t2(&A::g, a);
    t1.join();
    t2.join();
}

// lock_guard is a wrapper class that provides a convenient RAII-style mechanism for owning 
// a mutex for the duration of a scoped block. When a lock_guard object is created, it attempts 
// to take ownership of the mutex it is given. When control leaves the scope in which the 
// lock_guard object was created, the lock_guard is destructed and the mutex is released.