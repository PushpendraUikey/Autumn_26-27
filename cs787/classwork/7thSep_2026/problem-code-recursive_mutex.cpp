#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

recursive_mutex m;
int f(int x) {
 int tmp;
	m.lock();
	if (x==1) {
		cout << this_thread::get_id()<<":" << x << endl;
		this_thread::sleep_for(300ms);
		return 1;
	}
	else {
		tmp = x+f(x-1);
		cout << this_thread::get_id()<<":" << tmp << endl;
		this_thread::sleep_for(300ms);
		return (tmp);
	}
	m.unlock();
};

int main () {
 thread t1(f,10);
 thread t2(f,8);
 t1.join();
 t2.join();
}

