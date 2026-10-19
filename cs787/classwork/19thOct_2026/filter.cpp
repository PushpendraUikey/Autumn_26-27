#include <iostream>
#include <vector>
using namespace std;

template <typename T, typename F>
vector<T> filter(const vector<T>& vec, F func) {
    vector<T> result;
    for (const auto& item : vec) {
        if(func(item)) {
            result.push_back(item);
        }
    }
    return result;
}

int main() {
    vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto even_numbers = filter(numbers, [](int x) { return x % 2 == 0; });
    cout << "Even numbers: ";
    for (const auto& num : even_numbers) {
        cout << num << " ";
    }
    cout << endl;
}
//  filter (vector of type T, lambda expression) produces a vector of type T by applying the given
// lambda expression, picking all values for which the expression returns true
