#include <iostream>
#include <vector>
using namespace std;

template <typename T>
vector<T> map (vector<T> v, auto f) {
    vector<T> result;
    for (int i=0; i<v.size(); i++) {
        result.push_back(f(v[i]));
    }
    return result;
}

template <typename T, typename F>
vector<T> map2 (vector<T> v, F f) {
    vector<T> result;
    for (int i=0; i<v.size(); i++) {
        result.push_back(f(v[i]));
    }
    return result;
}

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    auto l1 = [] (int x) { return x * x; };
    auto l2 = [] (int x) { return x + 10; };

    auto result = map(v, l1);
    for (int i : result) {
        cout << i << " ";
    }
    cout << endl;
    result = map2(v, l2);
    for (int i : result) {
        cout << i << " ";
    }
    cout << endl;
    return 0;
}

//  map (..vector of type T..., a lambda expression ...) --> produces another vector of type T
// by applying the expression to each value inside the vector


