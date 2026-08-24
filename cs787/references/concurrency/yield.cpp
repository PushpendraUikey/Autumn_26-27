#include<iostream>
#include<thread>
using namespace std;

class A {
public:
    void f(int i) {
        while(1) {
            cout << "hello " << i++ << endl; 
            if (i%10) this_thread::yield();
        }
    } 
};

class B {
public:
    void g(int i) {
        while (1) {
            cout << "hi " << i++ << endl;
            if (i%20) this_thread::yield();
        }
    }
};

int main () {

A *a = new A();
B *b = new B();

thread t1 {& A::f, a, 0};
thread t2 {& B::g, b, 0};

t1.join();
t2.join();

}
