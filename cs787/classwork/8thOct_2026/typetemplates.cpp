// type template parameters

#include <iostream>
#include <vector>
using namespace std;

template <typename T>
class TQ {
 public:
	TQ <T> &operator << (const T& e){
		v.push_back(e);
		return *this;
	};
	const T& operator [] (int i){
		return v[i];
	}; 
	private:
		vector<T> v;
};


int main () {

	int N=10;
	TQ<int> Q;
		for (int i=0; i<N; i++) 
			Q << i << i*i;	

		for (int i=0; i<N*2; i++) 
			cout << Q[i] << endl;

	TQ<string> S;
		for (int i=0; i<N; i++) 
			S << "hello" << "world";	

		for (int i=0; i<N*2; i++) 
			cout << S[i] << endl;
	//....
}



