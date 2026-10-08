#include <iostream>
#include <queue>
using namespace std;

int main () {

	queue <int> q;
	int e;

	q.push(10);
	q.push(20);
	q.push(30);

	while (!q.empty()) {
		cout << "  q size:" << q.size() << endl;
		e = q.front();
		cout << e << endl;
		q.pop();
	}

};
