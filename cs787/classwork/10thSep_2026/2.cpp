#include <iostream>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace std;

/*
Don't keep lock while sleep. 
Use bool variable's to check conditions inside critical section(using critical block var), then 
use it to call notify on condition variable. 
-- Don't call notify() while holding the lock. Although this works but extra overhead of switching ctx bw 
-- cv queue and mutex queue
Make blocks to use critical section variables only and rest of code should be out of lock.
*/

long counter = 0;
int workers_done = 0;
int f_done = 0;

mutex m;
condition_variable cv_phase1;
condition_variable cv_phase2;

void f() {
    {
        unique_lock<mutex> ul(m);
        for(int i=0; i<4; i++) counter++;
        cout  << "Phase 1 f() done:\n";
        f_done = 1;
    }
    cv_phase1.notify_all();
    {
        unique_lock<mutex> ul(m);
        cv_phase2.wait(ul, []{ return workers_done >= 4; });

        counter = 2 * counter;
        cout << "Final counter value: " << counter << '\n';
    }

}

void g() {
    {
        unique_lock<mutex> ul(m);
        cv_phase1.wait(ul, []{ return f_done==1; });
    }

    {
        lock_guard<mutex> lg(m);
        counter++;
    }
    this_thread::sleep_for(100ms);
    bool is_last_worker = false;
    {
        lock_guard<mutex> lg(m);
        counter++;
        workers_done++;
        
        if(workers_done == 4) is_last_worker = true;
        std::cout << "worker G done\n";
    }

    if(is_last_worker) cv_phase2.notify_one();
}

int main(int argc, char* agrv[]) {
    thread t(f);
    thread t1(g);
    thread t2(g);
    thread t3(g);
    thread t4(g);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t.join();
}

/*
long counter = 1;
int job = 0;
mutex mtx;
std::condition_variable cvF, cvG;

void f() {
    unique_lock<mutex> ul(mtx);
    for (int i=0; i<4; i++) counter++;

    cvF.wait(ul, []{ return job == 4;});

    counter = 2 * counter;
    cout << counter << endl;
    cvG.notify_all();
    cout << "F completed Execution\n";
}

void g() {
    unique_lock<mutex> ul(mtx);
    counter++;
    this_thread::sleep_for(100ms);
    counter++;

    job++;
    cvF.notify_one();
    cvG.wait(ul);
    cvG.notify_all();
    cout << "G finished Execution\n";
}

int main() {
    thread t(f);
    this_thread::sleep_for(100ms);

    thread t1(g);
    thread t2(g);
    thread t3(g);
    thread t4(g);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    t.join();

    return 0;
}

1. Complete Serialization of Worker Threads (The "Sleep Under Lock" Bug)
In g(), you hold the unique_lock<mutex> ul(mtx) for the entire duration of the function, including during 
this_thread::sleep_for(100ms).

The Result: Your workers do not run concurrently. Thread t1 locks the mutex, increments, sleeps 100ms, increments, 
and yields. Only then can t2 start.

Impact: Instead of 4 threads sleeping concurrently (taking ~100ms total), they execute strictly sequentially, taking 
400ms total. You have accidentally turned a multithreaded program into a single-threaded sequence.

2. The Startup Race Condition (The "Sleep Hack")
In main(), you use this_thread::sleep_for(100ms) to ensure f() grabs the lock before the g() threads start.

The Result: This is a race condition. Thread scheduling is determined by the OS, not by time. If the system is under heavy 
load and thread t takes 150ms to spawn, one of the g threads will grab the lock first, completely violating the requirement 
that f executes its increments first.

The Fix: You must use a condition variable (like cv_phase1 in my previous example) or a boolean flag to strictly enforce that 
f finishes its first phase before g threads are allowed to proceed.

3. Vulnerability to Spurious Wakeups
In g(), you use cvG.wait(ul); without a predicate.

The Result: Condition variables can wake up randomly without being notified (spurious wakeups). If a spurious wakeup occurs, 
a g thread will prematurely exit the wait state, print "G finished Execution", and terminate before f has actually finished 
doubling the counter.

The Fix: Always wait with a predicate: cvG.wait(ul, []{ return f_is_done; });.

4. The "Hurry Up and Wait" Anti-Pattern
In g(), you call cvF.notify_one() while still holding the unique_lock.

The Result: As discussed previously, Thread f will wake up, immediately try to acquire mtx, fail 
(because g hasn't reached cvG.wait(ul) to release it yet), and go back to sleep on the mutex queue.

5. Unnecessary Cascade Notifications
In g(), after waking up from cvG.wait(ul), you call cvG.notify_all().

The Result: This is redundant. Thread f already called cvG.notify_all(), which woke up all the g threads. 
Having every g thread wake up and then needlessly spam notify_all() to the others creates unnecessary overhead and 
lock contention at the very end of your program.
*/