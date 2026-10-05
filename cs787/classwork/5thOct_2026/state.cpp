#include <bits/stdc++.h>
using namespace std;


class LIFO;

class LIFOState {
public:
	virtual int add(int x, LIFO *lifo) = 0;
	virtual int fetch(LIFO *lifo) = 0;
	virtual ~LIFOState() = default;
};

class FULL;
class EMPTY;
class PARTIAL;
class LIFO {
	LIFOState *current;
	LIFOState *fullstate;
	LIFOState *emptystate;
	LIFOState *partialstate;
	vector <int> buffer;
	friend class FULL;
	friend class EMPTY;
	friend class PARTIAL;
	public:
		LIFO(int cap);
		int add(int x);
		int fetch();
	private:
		void changeState() {
			if (buffer.empty()) current = emptystate;
			else if (buffer.size() == buffer.capacity()) current = fullstate;
			else current = partialstate;
		}
};

class FULL : public LIFOState {
	int add(int, LIFO *) override { 
		cout << "Buffer is FUll\n";
		return -1; }
	int fetch(LIFO *lifo) override {
		int x = lifo->buffer.back();
		lifo->buffer.pop_back();
		return x;
	}
};

class EMPTY : public LIFOState {
	int add(int x, LIFO *lifo) override {
		lifo->buffer.push_back(x);
		return x;
	}
	int fetch(LIFO *) override {
		cout << "Buffer is empty" << endl;
		return -1;
	}
};

class PARTIAL : public LIFOState {
	int add(int x, LIFO *lifo) override {
		lifo->buffer.push_back(x);
		return x;
	}
	int fetch(LIFO *lifo) override {
		int x = lifo->buffer.back();
		lifo->buffer.pop_back();
		return x;
	}
};

LIFO::LIFO(int cap) {
	buffer.reserve(cap);
	fullstate = new FULL();
	emptystate = new EMPTY();
	partialstate = new PARTIAL();
	current = emptystate;
}

int LIFO::add(int x) {
	int res = current->add(x, this);
	changeState();
	return res;
}

int LIFO::fetch() {
	int res = current->fetch(this);
	changeState();
	return res;
}

int main () {

LIFO *bb = new LIFO (4);
	cout << bb->fetch() << endl;
	cout << bb->add(1) << endl;
	cout << bb->add(2) << endl;
	cout << bb->add(3) << endl;
	cout << bb->fetch() << endl;
	cout << bb->add(4) << endl;
	cout << bb->add(5) << endl;
	cout << bb->add(6) << endl;
};
