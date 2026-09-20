/*
3 resources, a,b,c
thread 1 -- only a
thread 2 -- only b
thread 3 -- only c
thread 4 -- a,b
thread 5 -- b,c
thread 6 -- a,c
thread 7 -- a,b,c
each thread prints its id (1 .. 7) 3 to 4 times in 200ms gap

permitted overlapping:
1,2,3
1,2
etc.

We should not prevent any permissible overlap.
*/


#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <semaphore>

using namespace std;

class Resource {
    public:
    int a = 0, b = 0, c = 0;

    // counting_semaphore<1> sema{1};
    // counting_semaphore<1> semb{1};
    // counting_semaphore<1> semc{1};
    mutex lcka,lckb,lckc;

    void increment_and_print(int id, int& val, string var_name) {
        val++;
        cout << "Thread " << id << ": "
             << val << " (" << var_name << ")" << endl;
    }

    int access(int id, string binary_id) {
        if (binary_id[0] == '1')
            lcka.lock();
        if (binary_id[1] == '1')
            lckb.lock();
        if (binary_id[2] == '1')
            lckc.lock();
        for (int i = 0; i < 4; i++) {
            if (binary_id[0] == '1')
                increment_and_print(id, a, "a");
            if (binary_id[1] == '1')
                increment_and_print(id, b, "b");
            if (binary_id[2] == '1')
                increment_and_print(id, c, "c");
            this_thread::sleep_for(chrono::milliseconds(2000));
        }
        if (binary_id[2] == '1')
            lckc.unlock();
        if (binary_id[1] == '1')
            lckb.unlock();
        if (binary_id[0] == '1')
            lcka.unlock();
        return 0;
    }
};

int main() {
    Resource r;

    thread t1(&Resource::access, &r, 1, "100");
    thread t2(&Resource::access, &r, 2, "010");
    thread t3(&Resource::access, &r, 3, "001");
    thread t4(&Resource::access, &r, 4, "110");
    thread t5(&Resource::access, &r, 5, "011");
    thread t6(&Resource::access, &r, 6, "101");
    thread t7(&Resource::access, &r, 7, "111");

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();

    return 0;
}