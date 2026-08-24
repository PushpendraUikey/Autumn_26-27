#include<iostream>
#include<thread>
#include <mutex>
using namespace std;

mutex mtx;

class A { 
public:

void f(int i) {mtx.lock(); while(i++<100) cout << "hello " << i << endl; mtx.unlock();} 

};

class B {
public:
void g(int i) {mtx.lock(); while (i++<300) cout << "hi " << i << endl; mtx.unlock();}
};

int main () {

A *a = new A();
B *b = new B();

thread t1 {& A::f, a, 0};
thread t2 {& B::g, b, 200};

t1.join();
t2.join();

}
