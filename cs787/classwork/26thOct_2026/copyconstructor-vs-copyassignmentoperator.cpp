#include <iostream>
using namespace std;

class Student {
public:
    int* marks;

    Student(int m) {
        marks = new int(m);
    }

    // Copy constructor
    Student(const Student& other) {
        cout << "Copy constructor called\n";
        marks = new int(*other.marks+20);
    }

    // Copy assignment operator
    Student& operator=(const Student& other) {
        cout << "Copy assignment operator called\n";
        if (this != &other) {
            *marks = *other.marks+30;
        }

        return *this;
    }
};

int main() {
    Student s1(90);

    Student s2 = s1;  // Copy constructor, object is being constructed for
		// the first time
    cout << "s1 marks: " << *s1.marks << endl;
    cout << "s2 marks: " << *s2.marks << endl;

    Student s3(70); // no copy constructor, object being created for first time
    cout << "s3 marks: " << *s3.marks << endl;

    s3 = s1;          // Copy assignment operator, object already exists

    cout << "s3 marks: " << *s3.marks << endl;

    return 0;
}
