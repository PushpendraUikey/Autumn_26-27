#include <iostream>
#include <unordered_map>
using namespace std;

template <class KEY, class VAL>
class AssociativeArray {
	unordered_map<KEY, VAL> m;
	public:
		AssociativeArray &add (KEY k, VAL e){
			m[k] = e;
			return *this;
		};
		KEY operator [] (VAL k){
			for (auto &x : m) {
				if (x.second == k) return x.first;
			}
			return KEY();
		};

};


int main () {

	string s1("hello");
	string s2("hello");
	cout << (s1 <s2) << endl;
	cout << (s1 >s1) << endl;
	cout << (s1==s2) << endl;


AssociativeArray<string,string> a;
	a.add(string("Adya"), string("proper noun"));
	a.add(string("a"), string("article"));
	a.add(string("table"), string("noun"));
	cout << a["noun"] << endl;


}
