#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex f1, f2;

void P1() {
   while (1) {
	f1.lock();
	f2.lock(); 
		cout << "Phil 1 eating\n";
		this_thread::sleep_for(300ms);
	f1.unlock();
	f2.unlock();
	this_thread::sleep_for(100ms);
   }
}
void P2() {
   while (1) {
	if (f2.try_lock()){
		if(!f1.try_lock()) {
			f2.unlock();
			this_thread::sleep_for(100ms);
			continue;
		} 
		cout << "Phil 2 eating\n";
		this_thread::sleep_for(300ms);
		f1.unlock();
		f2.unlock();
	}
	this_thread::sleep_for(100ms);
   }
}

int main () {

 thread t1(P1);
 thread t2(P2);
 t1.join();
 t2.join();

};
