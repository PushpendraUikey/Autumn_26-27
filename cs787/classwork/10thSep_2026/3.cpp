#include <bits/stdc++.h>
#include <condition_variable>
#include <mutex>
#include <thread>
using namespace std;

mutex m;
condition_variable cv_phase1;
condition_variable cv_phase2;
int workers_done = 0;
int counter = 0;

void f() {
    int count = 0;
    for (int i=0; i<4; i++) {
        count++;
    }
    {
        unique_lock<mutex> ul(m);
        workers_done++;
        cv_phase1.wait(ul, []{ return workers_done==3; });
        cv_phase1.notify_one();
    }
    
    {
        lock_guard<mutex> lg(m);
        counter += count;
    }
    this_thread::sleep_for(10ms);
    {
        lock_guard<mutex> lg(m);
        std::cout << "Worker has counter: " << counter << endl;
    }
}

int main(int argc, char* argv[]) {
    thread t1(f);
    thread t2(f);
    thread t3(f);

    t1.join(); t2.join(); t3.join();
}