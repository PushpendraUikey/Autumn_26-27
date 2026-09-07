#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

mutex mtx;
condition_variable cv;
queue<int>buff;

int A[5];
int NITEMS=0;
int CAP=5;
int item=0;

/*
predicate on the condition variable is: If predicate is false wait, else continue
Predicate TRUE: continue right away, the thread ignores the wait entirely.
Predicate FALSE: wait until notified and the predicate is true.
*/

void  produce () {
   while (1) {
    unique_lock<mutex> ul (mtx);  // it works like lock guard, and no need to specify
    cv.wait (ul, []{return NITEMS<CAP;});
    cout << "producing " << item << endl;
    buff.push(item++);
    NITEMS++; 
    ul.unlock();
    cv.notify_all();
    this_thread::sleep_for(300ms);
  }
}

void consume () {
   while (1) {
    unique_lock<mutex> ul (mtx);  // it works like lock guard, and no need to specify
    cv.wait (ul, []{return NITEMS!=0;});
    cout << "consuming" << buff.front() << endl;
    buff.pop();
    NITEMS--; 
    ul.unlock();
    cv.notify_all();
    this_thread::sleep_for(300ms);
  }
}


int main () {

thread t1 (produce);
thread t2 (consume);

t1.join();
t2.join();

}
