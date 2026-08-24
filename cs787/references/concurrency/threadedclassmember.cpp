#include<iostream>
#include<thread>
using namespace std;

class A {
public:
void f() {int i=0; while (i++<100) cout << "hello " << i << endl;} 
};

class B {
public:
void g() {int i=200; while (i++<300) cout << "hi " << i << endl;}
};

int main () {

    A *a = new A();
    B *b = new B();

    thread t1 {& A::f, a};
    thread t2 {& B::g, b};

    t1.join();
    t2.join();

}
