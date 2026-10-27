#include <iostream>
#include <vector>
using namespace std;

// If base class has non-virtual destructor then when program deletes the base class pointer which points to
// the child class pointer, the destructor of the child class may never called which leaves memory footprints
class A {
public:
	~A() {cout << "destroying A obj\n";} 
};
class B: public A {
public:
	~B() {cout << "destroying B obj\n";}
};


int main () {

 A *ap = new B();

 delete (ap); // undefined behavior, B's destructor may not get called
	vector<int*> arr;
	int *s1 = new int(5);
	arr.push_back(s1);

	vector<int*> brr(arr); // copy constructor

	cout << *arr[0] << endl;
	cout << *brr[0] << endl;

	cout << endl;

	*brr[0] = 45;
	cout << *arr[0] << endl;
	cout << *brr[0] << endl;
}
