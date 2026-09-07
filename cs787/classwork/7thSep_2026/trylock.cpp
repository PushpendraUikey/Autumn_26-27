#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex m;
void f() {
  while (1) {
	while (!m.try_lock());
	cout << "f ...\n";
	m.unlock();
	this_thread::sleep_for(300ms);
  }
}
void g() {
  while (1) {
	while (!m.try_lock());
	cout << "g ...\n";
	m.unlock();
	this_thread::sleep_for(300ms);
  }
}
int main () {
thread t1(f);
thread t2(g);
 t1.join(); t2.join();
}
