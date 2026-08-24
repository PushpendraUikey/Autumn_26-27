#include<iostream>
#include<thread>
using namespace std;

void f() {int i=0; while (i++<10) {cout << "hello " << i << endl; 
			this_thread::sleep_for (2s);}}
void g() {int i=20; while (i++<30) {cout << "hi " << i << endl; 
			this_thread::sleep_for(1s);}}

int main () {

thread t1 {f};
thread t2 {g};

t1.join();
t2.join();



}
