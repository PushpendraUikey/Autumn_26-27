#include <iostream>
#include <thread>

using namespace std;

void f() {  cout << "Hello\n";}
void g() {  cout << "World\n";}

int main(int argc, char *argv[]) {
    thread t1(f);
    thread t2(g);

    //t1.join();
    //t2.join();

    cout << "Finished main\n";
    t2.join();
    t1.join();
}

/*
thread::join() allows the main thread to wait for the completion of the threads t1 and t2 before proceeding.
join() allows the main thread to block until the thread on which join() is called has finished executing. 
This ensures that the main thread does not exit before the other threads have completed their work, 
preventing potential issues such as accessing resources that have been cleaned up or terminated prematurely.
*/