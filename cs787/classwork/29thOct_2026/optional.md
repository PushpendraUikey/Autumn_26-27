std::optional<> is a template class introduced in C++17 that represents an optiona value - meaning a value that may or may not be present.

It acts as a type-safe wrapper around an object of type T, eliminating the need for error-prone walkarounds like magic numbers (eg. returning 0, -1). or raw pointer null checks.


Key Features:
1. Value storage: std::optional<T> stores the object T directly inside itself without requiring dynamic memory or heap allocation.
2. States: At any given time, an instance either contains a value of type T or is empty (represented by std::nullopt)
3. Checking presence: Use has_value() to check if a value exists.
4. Accessing the Value:
*opt, or opt-> (dereferncing like a pointer).
opt.value() (accesses the value, or throws a std::bad_optional_access)


This might be equivalent to the Maybe Monad in functional program Haskell.