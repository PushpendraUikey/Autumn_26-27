
A(const A& a, int v) is not actually a copy constructor according to C++ rules.

In C++, a constructor only qualifies as a copy constructor if its first argument is a reference to class type (like const A&) and all subsequent arguments have have default values. 
The above one is treated as a parameterized constructor.

1. Default Arguments create Copy Constructors: If signature is changed to A(const A& a, int v=0), it immediately qualifies as a copy constructor.

2. The Default Constructor Rule: Defining any constructor like  A(int x) completely suppresses the implicit zero argument default constructor A().

3. Move Constructors Suppresses the Copying: If a move constructor is defined A(A&&), compiler stops generating the implicit copy constructor.

4. Assignment Operator Signatures: A custom assignment operator with a non standard signature, it will not suppress the compiler generated operator=(const A&)


