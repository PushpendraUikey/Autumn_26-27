#include <iostream>
#include <cstdarg>
using namespace std;

void print(int count, ...) {
    va_list args; // cursor over the unnamed arguments
    va_start(args, count); // starts the cursor just after the last named
    //parameter. That's why atleast one named parameter is needed.

    for (int i = 0; i < count; i++) {
        // reads the next argument and assumes it is an int.
        cout << va_arg(args, int) << " ";
    }
    
    // Does the cleanup and it is required.
    va_end(args);
    cout << "\n";
}
int main() {
    printf("%d\n", 10);
    printf("%d %d\n", 10, 20);
    printf("%d %d %d\n", 10, 20, 30);
    /*
    This is the old C-style variadic function, built with ... and <cstdarg>.
    This is standard library's own C variadic function.
    */
    print(3, 10, 20, 30);
}
