#include<iostream>
#include<thread>
using namespace std;

void f() {int i=0; while (i++<100) cout << "hello " << i << endl;} 
void g() {int i=200; while (i++<300) cout << "hi " << i << endl;}

int main () {

thread t1 {f};
thread t2 {g};

t1.join();
t2.join();



}
