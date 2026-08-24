#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

mutex lck;

class Event {
    int val;
    public:
    Event() : val(0) {}
    Event(int v) : val(v) {}
    void print() {
        cout << "Event :" << val << endl;
    }
};

template <typename T>
class Cirqueue {
    T arr[10];
    int front_index, rear;
    public:
        Cirqueue() : front_index(0), rear(0) {}
        void push(const T& item) {
            if((rear + 1) % 10 == front_index) {
                cout << "Queue is full\n";
                return;
            }
            arr[rear] = item;
            rear = (rear + 1) % 10;
        }
        void pop() {
            if(empty()) {
                cout << "Queue is empty\n";
                return;
            }
            front_index = (front_index + 1) % 10;
        }
        bool empty() const {
            return front_index == rear;
        }
        int size() const {
            return (rear - front_index + 10) % 10;
        }
    
        T front() {
            return arr[front_index];
        }
};

Cirqueue<Event>* q;

class Producer {
    int ctr = 0;
    public:
    void produce() {
        while(true) {
            lck.lock();
            while(q->size() < 9) {
                Event e(ctr++);
                q->push(e);
            }
            lck.unlock();
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

class Consumer {
    public:
    void consume() {
        while(true) {
            lck.lock();
            while(q->empty()) {
                lck.unlock();
                this_thread::sleep_for(chrono::milliseconds(100));
                lck.lock();
            }
            Event e = q->front();
            q->pop();
            e.print();
            lck.unlock();
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
};

int main( int argc, char* argv[]) {

    q = new Cirqueue<Event>();

    Producer *p = new Producer();
    Consumer *c = new Consumer();

    thread t3(&Producer::produce, p);
    thread t4(&Consumer::consume, c);

    cout << "Id of thread t3: " << t3.get_id() << endl;
    cout << "Id of thread t4: " << t4.get_id() << endl;

    t3.join();
    t4.join(); 
}