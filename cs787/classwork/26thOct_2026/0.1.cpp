#include <iostream>
using namespace std;

class Student {
public:
    int* marks;

    Student(int m) {
        marks = new int(m);
    }
};

int main() {
    Student s1(90);
    Student s2 = s1;  // Compiler-generated copy constructor (Since the object is being created, hence copy 
                      // constructor instead of assignment operator - Also shallow copy)

    *s2.marks = 50;
    cout << *s1.marks << endl;  // 50
    cout << *s2.marks << endl;  // 50

    *s2.marks = 70;
    cout << *s1.marks << endl;  // 70
    cout << *s2.marks << endl;  // 70

    int *ptr = new int(100);
    cout << "Before Deleting ptr: " << ptr  << " and pointer value: " << *ptr << endl;
    delete ptr;
    cout << "After Deleting ptr: " << ptr << " and pointer value: " << *ptr << endl;
    return 0;
}
