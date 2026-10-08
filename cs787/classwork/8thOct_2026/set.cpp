#include <iostream>
#include <concepts>
#include <string>
#include <set>
using namespace std;

// compile using the following command - use C++20 and above standard to support concepts
// g++ -std=c++20 set.cpp 

template <typename T>
concept Printable = requires(std::ostream& os, const T& val) {
	os << val;
};


template <typename T>
class bigSet {
	public:
		set <T> s;
	public:
		bigSet() { cout << "bigSet constructor" << endl; }
		~bigSet() { cout << "bigSet destructor" << endl; }

		bigSet(const bigSet &a) {
			cout << "bigSet copy constructor" << endl;
			s = a.s;
		}
		bigSet(bigSet &&a) {
			cout << "bigSet move constructor" << endl;
			s = std::move(a.s);
		}

		// union
		bigSet operator+(const bigSet &a) {
			bigSet res;
			res.s = s;
			for (auto &e: a.s){
				res.s.insert(e);
			}
			return res;
		}

		void push(const T &e) {
			s.insert(e);
		}

		void pop(const T &e) {
			if(s.empty()) {
				throw std::out_of_range("bigSet is empty");
			}
			s.erase(e);
		}

		T top() {
			if(s.empty()) {
				throw std::out_of_range("bigSet is empty");
			}
			return *s.begin();
		}

		// difference
		bigSet operator-(const bigSet &a) {
			bigSet res;
			res.s = s;
			for (auto &e: a.s){
				res.s.erase(e);
			}
			return res;
		}

		// intersection
		bigSet operator*(const bigSet &a) {
			bigSet res;
			for (auto &e: s){
				if (a.s.find(e) != a.s.end()){
					res.s.insert(e);
				}
			}
			return res;
		}

		// bigSet map
		template <typename F>
		bigSet map(F f) {
			bigSet res;
			for (auto &e: s) {
				res.s.insert(f(e));
			}
			return res;
		}

		// bigSet filter
		template <typename F>
		bigSet filter(F f) {
			bigSet res;
			for (auto &e: s) {
				if(f(e)) {
					res.s.insert(e);
				}
			}
			return res;
		}

		template <typename F>
		T reduce(T init, F f) {
			T res = init;
			for (auto &e: s) {
				res = f(res, e);
			}
			return res;
		}

		void print() requires Printable<T>{
			for (auto &val: s){
				std::cout << val << " ";
			}
			std::cout << std::endl;
		}

};

int main () {

set <int> s;
int e;

	s.insert(10);
	s.insert(10);
	s.insert(20);
	s.insert(30);

	for (auto &e: s)
		cout << e << endl;

	bigSet<int> bs1;
	bigSet<int> bs2;

	bs1.push(10);
	bs1.push(20);
	bs1.push(30);
	bs1.pop(20);
	bs1.push(50);


	bs2.push(20);
	bs2.push(30);
	bs2.push(100);
	bs2.push(200);

	bigSet<int> bs3 = bs1 + bs2;
	cout << "+: ";
	bs3.print();

	bigSet<int> bs4 = bs1 - bs2;
	cout << "-: ";
	bs4.print();

	bigSet<int> bs5 = bs1 * bs2;
	cout << "*: ";
	bs5.print();

	bigSet<int> bs6 = bs1.map([](int x) { return x * 2; });
	cout << "map: ";
	bs6.print();
	

	bigSet<int> bs7 = bs1.filter([](int x) { return x > 20; });
	cout << "filter: ";
	bs7.print();

	int sum = bs1.reduce(0, [](int acc, int x) { return acc + x; });
	cout << "reduce: " << sum << endl;

};

// your own template set class to support: +, - for elements and sets, intersection
// and support map, reduce, filter operations
