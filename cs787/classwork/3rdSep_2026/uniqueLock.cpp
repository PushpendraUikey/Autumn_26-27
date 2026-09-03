#include <bits/stdc++.h>
#include <thread>
using namespace std;

class A {
    mutex m;
    public: 
        void f() {
            std::cout << "f() is starting" << std::endl;
            unique_lock<mutex> ug(m);
            for (int i=0; i<4; i++) {
                cout << " ---- " << i << endl;
                if(i==2) {
                    ug.unlock(); // allowed: Unlocks the mutex.
                }
                if(i==2) {
                    ug.lock();  // allowed: Locks the mutex again.
                }
            }
            this_thread::sleep_for(chrono::microseconds(300));
            std::cout << "f() is done" << std::endl;
        }
        void g() {
            std::cout << "g() is starting" << std::endl;
            unique_lock<mutex> ug(m);
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