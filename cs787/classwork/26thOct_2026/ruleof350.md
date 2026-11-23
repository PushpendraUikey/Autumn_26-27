
A(const A& a, int v) is not actually a copy constructor according to C++ rules.

In C++, a constructor only qualifies as a copy constructor if its first argument is a reference to class type (like const A&) and all subsequent arguments have have default values. 
The above one is treated as a parameterized constructor.

1. Default Arguments create Copy Constructors: If signature is changed to A(const A& a, int v=0), it immediately qualifies as a copy constructor.

2. The Default Constructor Rule: Defining any constructor like  A(int x) completely suppresses the implicit zero argument default constructor A().

3. Move Constructors Suppresses the Copying: If a move constructor is defined A(A&&), compiler stops generating the implicit copy constructor.

4. Assignment Operator Signatures: A custom assignment operator with a non standard signature, it will not suppress the compiler generated operator=(const A&)


## Rule of 3
If a class requires custom implementation of any the following three special member functions, it almost certainly requires all three.
1. Destructor (~A())
2. Copy Constructor (A(const A&))
3. Copy Assignment Operator(A& operator=(const A&))

When a class directly manages raw pointers or system handles,
the compiler's implicit copy operations perform a shallow copy (duplicating the raw pointer addresses). This leads to shared state bugs, unexpected memory mutation, and double free exceptions when both instances run their destructors. 

<code>
class Buffer {
    int *data;
    size_t size;

public:
    Buffer(size_t s) : size(s), data(new int[s]){}

    // 1. Destructor
    ~Buffer() { delete[] data; }

    // 2. Copy Constructor (Deep Copy)
    Buffer(const Buffer& other): size(other.size), data(new int[other.size]) {
        std::copy(other.data, other.data+size, data);
    }

    // 3. Copy Assignment Operator
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new int[size];
            std::copy(other.data, other.data+size, data);
        }
        return *this;
    }
};
<code>


## Rule of 5
C++11 introduced the move semantics, which allowed temporary objects (rvalues) to transfer their owned resources rather deep copying them. Rule expanded to 5 member functions.

1. Destructor
2. Copy Constructor
3. Copy Assignment Operator
4. Move Constructor (A(A&&) noexcept)
5. Move Assignment Operator (A& operator=(A&&) noexcept)

Move constructor steal the resources from temporary objects and leave source object in a safe, destructible, empty state.

<code>
class Buffer{
    int* data;
    size_t size;

public:
    Buffer(size_t s): size(s), data(new int[s]) {}
    ~Buffer() { delete[] data; }

    // Copy operations
    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer& other);

    //4. Move Constructor
    Buffer(Buffer&& other) noexcept: data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }

    //5. Move Assignment Operator
    Buffer& operator=(Buffer&& other) noexcept {
        if(this != &other) {
            delete[] data;  // cleanup existing resource
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};

