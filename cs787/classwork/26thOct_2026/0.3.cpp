
#include <iostream>
using namespace std;

class Student {
public:
    int* marks;

    // Constructor
    Student(int m) {
        marks = new int(m);
    }

    // Shallow copy constructor
    // Copies the pointer address
    Student(const Student& other) {
    	marks = new int(*other.marks);
        // inside copy constructor, but doing 
		// deep copy
    }

    // Destructor
    ~Student() {
        // Comment this out for the initial shallow-copy demo
        // delete marks;
    }
};

int main() {
    Student s1(90);

    // Copy s1 into s2
    Student s2 = s1;

    cout << "Before modification:\n";
    cout << "s1 marks = " << *s1.marks << endl;
    cout << "s2 marks = " << *s2.marks << endl;

    // Modify s2's marks
    *s2.marks = 50;

    cout << "\nAfter modifying s2:\n";
    cout << "s1 marks = " << *s1.marks << endl;
    cout << "s2 marks = " << *s2.marks << endl;

    cout << "\nAddress stored in s1.marks: "
         << s1.marks << endl;
    cout << "Address stored in s2.marks: "
         << s2.marks << endl;

    return 0;
}

