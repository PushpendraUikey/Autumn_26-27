#include <iostream>
#include <vector>
#include<algorithm>

using namespace std;


int main () {


 vector<int> v;

      for (int i=0;i<10;i++)
	v.push_back(i);

      for_each (v.begin(), v.end(), [](int x) {cout << x*x << endl;});

 	// inlined functions which are anonymous functions
}
