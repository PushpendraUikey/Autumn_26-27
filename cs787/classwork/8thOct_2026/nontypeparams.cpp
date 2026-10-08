// using constants in template expressions

#include <iostream>
using namespace std;

template <class T, int S>
class TArray {
 public:
	T &operator [] (int i) {
		return data[i];
	}

 private:
	T data[S];
};
const int N = 10; // commenting N would cause an error in the template expression
// Since comilers needs to know at the compile time the values's of the template
// parameters to construct classes based on the template used.
// If N is not a constant, or it is a variable, then the compiler will not be able to 
// construct the class TArray<T, N> because it will not know the size of the array at compile time.
// Even if there were no array and S wan't even used in the class, still the compiler will 
// not be able to construct the class TArray<T, N> because it will not know the value of N at compile time.

int main () {
	TArray<int, N> A;

	// assign N values  -- for loop
	// print N values   -- for loop
	for(int i = 0; i < N; i++) {
		A[i] = i*i;
	}
	for(int i = 0; i < N; i++) {
		cout << A[i] << endl;
	}
}



