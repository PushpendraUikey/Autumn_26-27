
Abstraction -> reasoning about abstract data structures -> carefully designed operations -> exception safety.


1. Abstraction:
    We don't need to know whether the stack is internally implemented using a deque, array, linked list etc. We reason about it at the abstract level: Abstract Data Type (ADT)

    define the data type through its operations and their precise behavior rather than its implementation.

    Reasoning about a program without executing it.
        -> Similar to what we do in a mathematical analysis

        -> Deducing the results without executing anything.


2. Comes the intresting STL Desing: pop() returns void
    A strong STL design decision. Strongly related to exception safety. std::stack::top() accesses the top element, while pop only removes it.


3. Why would T pop() be dangerous?

Suppose:
    class T {
        public:
            T(const T&){
                // May throw
            }
    }
Imagine:
    T pop();

Internally:
    1. Get top element.
    2. Remove it from stack
    3. Copy/move it into the return object
    4. Return it

Step 3 thows an exception:
    Before: Stack = [A B C]
    pop();
    removes C
    Stack = [A B]
    copy C into return object -> throws!
    Result:
        Stack = [A B]
    Returned value = nothing
    C = Lost

Problem:
    The element has already been removed, but constructing the return value may thow. How to Gracefully handle this exact failure mode -> Don't construct anything new and return void.


4. top() and pop() solve it.
    if you need the element you can succesfully take it using top then only remove it, since top copies and returns it so the program can recover the state even though there was an exception raised during the copying.


    Command-Query Separation style of interface design. Though the primary reason to make pop() return void was to handle the exception safety.



5. Mathematical Reasoning:
    Operations have a well defined semantics, we reason about a program by composing those operations.

    reason about behavior, not representation

6. Copying preserves the source object and produces an independent object with the same value. Moving transfers resources and may change the source, but the moved-from standard-library object remains valid, with its exact state generally unspecified.



7.
                 DATA ABSTRACTION
                       │
                       ▼
                Abstract Data Type
                       │
          ┌────────────┴────────────┐
          ▼                         ▼
   Ignore implementation       Define operations
          │                         │
          │                         ▼
          │                  Precise semantics
          │                         │
          └──────────────┬──────────┘
                         ▼
              We can mathematically
              reason about programs
                         │
                         ▼
                  STL containers
                         │
                         ▼
             Carefully designed APIs
                         │
                         ▼
                 Exception safety
                         │
                         ▼
       Why std::stack has separate
             top() and pop()
                         │
             ┌───────────┴───────────┐
             ▼                       ▼
          top()                    pop()
       observe value            modify state
          │                       │
       may throw               doesn't return
       while copying           a potentially
       value                    throwable value