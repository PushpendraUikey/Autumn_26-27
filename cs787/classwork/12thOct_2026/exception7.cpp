#include<iostream>
using namespace std;

class Engine {
 int capacity;
public:
	Engine (int x) {capacity = x;}
	~Engine () { 
		try {
			cout << "removing engine\n";  throw 1;}
		catch (...){
			cout << "engine removal failed\n";
		}
	}
};

class Train { 
	Engine e;
  public:
	Train () : e(100) {}
};
class HighSpeedTrain { 
	Engine e1;
	Engine e2;
  public:
	HighSpeedTrain () : e1(100),e2(200) {}
};
int main() {
  try {cout << "creating slow train\n"; Train t; cout << "created slow\n";}
  catch (...) {cout << "slow train junked\n";}
  try {cout << "creating fast train\n"; HighSpeedTrain t; 
	cout << "created speed\n";}
  catch (...) {cout << "speed train junked\n";}
}

// C++11 and Later Default (noexcept): User-defined destructors are implicitly marked noexcept(true). 
// This means the C++ compiler assumes destructor will never throw an exception to the outside world.
// If an exception tries to leave a destructor, the C++ runtime immediately calls std::terminate(), 
// which kills the process.

// If an exception is thrown while the call stack is already unwinding from a previous exception, C++
// instantly kills the process.


// Catch and swallow/log: Wrap risky cleanup code in a try-catch block inside the destructor itself and 
// log the error or store a failure flag
// Exception is swallowed and it never escapes the destructor.
