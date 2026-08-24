#include<iostream>
#include<thread>
using namespace std;

class A {
    public:
    void f(int i) {
        while (i++<100) 
            cout << "hello " << i << endl;
    } 
};

class B {
    public:
    void g(int i) {
        while (i++<300) 
        cout << "hi " << i << endl;
    }
};

int main () {

A *a = new A();
B *b = new B();

thread t1 {& A::f, a, 90};
thread t2 {& B::g, b, 298};

t1.join();
t2.join();
}
