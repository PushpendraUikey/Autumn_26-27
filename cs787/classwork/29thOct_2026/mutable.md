In C++ declaring a class member as mutable allows that specific variable to be modified insinde const member functions and on const objects.

Ordinarily, marking a member function as const promises the compiler that the function will not change any of the class's non-static data members. 


-> Bitwise Constness : means not a single bit inside the object's memory changes.
-> Logical Constness : means the observable, external state of the object appears unchanged though the underlying bookeeping or performance data shifts.


Help it provides:

1) Caching computed results (Lazy Evaluation):
    If a const function needs to calculate an expensive value and then store it so future calls are instant.
2) Access Counter, Metrics, or Logging:
    Might want to track how many times a const getter method is being called, without violating the const contract of the class.
3) Internal Synchronization and Mutexes:
    In multi-threaded classes a const getter might still need to lock a mutex to ensure thread-safe read access. Because locking a mutex mutates the mutex object itself hence mutex is often defined as mutable std::mutex


Rules & Limitations:
1. Cannot be static, const or reference.
2. Use with restraint.