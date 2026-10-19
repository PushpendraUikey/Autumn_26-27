#include <iostream>
#include <vector>
using namespace std;

template <typename T, typename F>
T reduce (vector<T> v, F f, T init) {
    T result = init;
    for (int i=0; i<v.size(); i++) {
        result = f(result, v[i]);
    }
    return result;
}

int main() {

    vector<int> v = {1, 2, 3, 4, 5};
    auto l1 = [] (int x, int y) { return x + y; };
    auto l2 = [] (int x, int y) { return x * y; };

    int sum = reduce(v, l1, 0);
    cout << "Sum: " << sum << endl;

    int product = reduce(v, l2, 1);
    cout << "Product: " << product << endl;

    return 0;
}
//   reduce (vector of a type, lambda expression) --> collapses the vector of type T values 
//  to one value of type T

 
