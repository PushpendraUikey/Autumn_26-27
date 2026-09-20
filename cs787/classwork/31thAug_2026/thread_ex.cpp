#include <iostream>
#include <thread>
using namespace std;

class A{
    int c, v;
    public:
    A(){c=1; v=0;}

    void f(){
        while(c){
            cout << "hello" << v++ << endl;
            this_thread::sleep_for(200ms);
        }   
        cout << "f gracefully exited" << endl;
    }
    void monitor(){
        int threshold = 15;
        bool flag = 0;
        while(true){
            if(v>=threshold){
                c=0;
                break;
            }
            else{
                this_thread::sleep_for(100ms);
            }
        }
    }
};

int main(){
    A *a = new A();
    thread t1 {&A ::f , a};
    thread t2 {&A ::monitor, a};

    t1.join();
    t2.join();
}