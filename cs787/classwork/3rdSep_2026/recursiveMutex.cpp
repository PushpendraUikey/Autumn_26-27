#include <bits/stdc++.h>
#include <thread>
using namespace std;

int counter = 11;
class A {
    recursive_mutex m;
    public: 
        void f() {
            unique_lock<recursive_mutex> ug(m);
            if(counter <= 0) {
                return;
            }
            std::cout << "f() with value " << counter << std::endl;
            counter--;
            this_thread::sleep_for(chrono::microseconds(300));
            f();
        }
        void g() {
            unique_lock<recursive_mutex> ug(m);
            if(counter <= 0) {
                return;
            }
            std::cout << "g() with value " << counter << std::endl;
            counter--;
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

/*
In this code, we are using a recursive_mutex instead of a regular mutex. A recursive_mutex allows 
the same thread to acquire the lock multiple times without causing a deadlock. This is particularly 
useful in recursive functions like f() and g(), where the same thread may need to lock the mutex 
multiple times as it calls itself recursively.
However one downside is single thread will do all the job, since it would recursively 
acquire the lock and again and again, so the other thread will not be able to acquire the lock until 
the first thread completely finishes its execution. So, the other thread will be blocked until the 
first thread releases the lock completely.

Also in this case we're using unique_lock Mutex Manager so we don't need to explicitly unlock the mutex, 
as unique_lock will automatically release the lock when it goes out of scope.

recursive_mutex keeps a reference count of how many times it has been locked by the same thread.
When thread unlocks the recursive_mutex, the reference count is decremented.
*/