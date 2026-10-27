#include<iostream>
using namespace std;

class A{
  public:
    A() {cout << "A constructor\n";};
    ~A() {cout << "A destructor\n";};
};
class B{
  public:
    B() {cout << "B constructor\n";};
    ~B() {cout << "B destructor\n";};
};
class C:public B{
  public:
    C() {cout << "C constructor\n";};
    ~C() {cout << "C destructor\n";};
};
class D{
  public:
    D() {cout << "D constructor\n";};
    ~D() {cout << "D destructor\n";};
};

void f(int x) {
 if (x==0) throw A();
 if (x==1) throw C();
 throw D();
};

int main() {

 try {
   int i;
   cin >> i;
   f(i);
 } 
 catch (const A& a) {cout << "caught A\n";}
 catch (const B& b) {cout << "caught B\n";}
 catch (const D& d) {cout << "caught D\n";}
 
}
