#include <bits/stdc++.h>
#include <thread>
using namespace std;

/*
This example correctly prevents any deadlock from occuring by unlocking the mutex before the recursive call, however
multiple threads will be able to acquire the lock and do the job, so it doesn't provide exclusivity to a single thread
like the recursive_mutex does. So, the output will be a mix of both threads doing the job, and not a single thread doing 
all the job.
*/

int counter = 11;
class A {
    mutex m;
    public: 
        void f() {
            if(counter <= 0) {
                return;
            }
            std::cout << "f() with value " << counter << std::endl;
            unique_lock<mutex> ug(m);
            counter--;
            ug.unlock();    // needed else deadlock
            this_thread::sleep_for(chrono::microseconds(300));
            f();
        }
        void g() {
            if(counter <= 0) {
                return;
            }
            std::cout << "g() with value " << counter << std::endl;
            unique_lock<mutex> ug(m);
            counter--;
            ug.unlock();    // needed else deadlock
            this_thread::sleep_for(chrono::microseconds(300));
            g();
        }
};

int main() {
    A *a = new A();
    thread t1(&A::f, a);
    thread t2(&A::g, a);
    t1.join();
    t2.join();
}