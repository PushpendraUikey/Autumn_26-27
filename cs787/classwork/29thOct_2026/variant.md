std::variant is a type safe, discrimated union that can hold exaclty one value from a predefined list of types at any given time. 

Unlike tradiational C-style unions, std::variant is type-safe: it continuously tracks which type is currently active. If you try to extract a wrong data type, it explicitly stops you by throwing a runtime exception rather than corrupting memory. It is frequently used for pattern matching, passing heterogeneous data type without dynamic memory allocation, and handling errors.


Key Features of std::variant
1. Type Safety: You cannot accidentally interpret a stored string as an integer. 
2. Stack Allocated: It does not use dynamic memory allocations (the heap), making it highly performant.
3. Object-Lifetime Aware: It automatically invokes proper constructors and destructors when the active type chagnes, preventing any memory leaks. 
4. Fixed Set of Types: You must delcare the permitted types as the compile time itself. 


Advanced matching: std::visit

The most powerful way to extract or process data inside a variant is via std::visit. It acts like a type-safe switch statement that executes a matching lambda or function depending on the active data type.


Important Constraints:
    No void or references: A variant cannot hold raw references (T&) or void types. You can use pointers (T*) or std::reference_wrapper if you need reference capabilities
    Default construction: By default, it initializes using the first type listed in template parameters. If the first type doesn't have a default constructor, compilation will fail.
    