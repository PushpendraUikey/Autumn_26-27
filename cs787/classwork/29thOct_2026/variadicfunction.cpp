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

/*
Important points which is why C++ added variadic templates:
1. No type safety. The function has no idea what types were actually passed. It trusts
the int in va_arg.
2. No count either. The caller must supply the count, as with count here or the format string
in printf.
3. Default promotions. char and short are promoted to int, and float is promoted to double.
so va_arg(args, char) and va_arg(args, float) are always wrong.
4. No class types are supported. bcz compiler doesn't know how to fit them into the unnamed 
arguments.
5. This all happens at runtime. There's one compile time function that walks the arguments at
runtime. Templates they are different: they generate a separate, type-checked function for each
call signature at compile time.
*/