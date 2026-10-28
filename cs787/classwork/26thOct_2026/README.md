# 26th Oct 2026 — Copy Semantics, Destructors & Casting

Quick recap of the lecture: copy constructor vs copy assignment, shallow vs
deep copy, virtual destructors, `dynamic_cast` vs `static_cast`, a
naming-service problem, and notes on copy elision.

## Files

| Topic | Files |
|---|---|
| Copy constructor: when it runs, default vs user-defined | `1.cpp`, `2.cpp`, `3.cpp` |
| Copy assignment operator: overloaded vs default | `4.cpp`, `5.cpp`, `copyconstructor-vs-copyassignmentoperator.cpp` |
| Shallow vs deep copy with pointer members | `0.1.cpp`, `0.2.cpp`, `0.3.cpp` |
| Virtual vs non-virtual destructor | `virdest.cpp`, `nonvdestr.cpp` |
| `dynamic_cast` | `1dyncast.cpp`, `2dynamic.cpp` |
| `static_cast` (and how it goes wrong) | `2.1statc.cpp`, `3staticcast.cpp` |
| Problem: naming service with safe casts | `PROBLEM`, `problemsolve.cpp` |
| Notes | `notes-copyconst-vs-copyassign.txt`, `default-copyconst-vs-defcopyassgnmentoperator.txt`, `copyElision.txt` |

## Key concepts

### Copy constructor vs copy assignment
- **Copy constructor**: runs when a **new** object is created from an
  existing one: `A c(a);` or `A c = a;`.
- **Copy assignment (`operator=`)**: runs when the object **already
  exists**: `c = a;`.
- The two can contain completely different logic. The examples use
  deliberately different arithmetic in each (`p*p`, `+20`, `+30`) so you can
  see from the output which one ran.
- If you don't write them, the compiler generates both, and both do a
  **member-by-member copy**.
- Writing *any* constructor removes the implicit **default constructor**, so
  `A f;` no longer compiles (`3.cpp`).
- `A(const A&, int v)` is not a copy constructor because it has an extra
  parameter with no default. That is why the implicit copy constructor is
  still generated and `A d(a);` still works (`3.cpp`).
- A user-defined `operator=` should check for self-assignment
  (`if (this != &other)`) and `return *this`.

### Shallow vs deep copy
- The default copy only **copies the pointer address**, so two objects end
  up sharing the same heap data. Changing one changes the other (`0.1`,
  `0.2`).
- A **deep copy** allocates new memory: `marks = new int(*other.marks);`
  (`0.3`).
- With a shallow copy, adding `delete marks` in the destructor would cause a
  **double delete**.
- The default assignment can also **leak** the memory the target object
  already owned, because it overwrites the pointer.
- `vector<int*>` copies the pointers, not the ints they point to. That is a
  shallow copy too (`nonvdestr.cpp`).
- Using a pointer after `delete` (dangling pointer) is undefined behaviour
  (end of `0.1.cpp`).

### Virtual destructors
- `delete basePtr;` on a derived object when the base destructor is
  **non-virtual**: only `~A()` runs, which is undefined behaviour and can
  leak resources (`nonvdestr.cpp`).
- With a **virtual** base destructor, `~B()` runs and then `~A()`, which is
  the correct cleanup (`virdest.cpp`).
- Rule: if a class is meant to be inherited from, give it a virtual
  destructor.

### Casting down a hierarchy
- Base pointer → derived object is implicit. Derived pointer ← base pointer
  does **not compile** without a cast.
- **`dynamic_cast<D*>(basePtr)`**: checks the type at **runtime** using
  RTTI.
  - It returns `nullptr` if the object isn't really a `D`. Always check the
    result.
  - It needs a **polymorphic** base (at least one virtual function).
- **`static_cast<D*>(basePtr)`**: checked at **compile time only**, with no
  runtime check. It never returns null for a wrong type.
  - Making sure the type is correct is the programmer's responsibility. A
    wrong cast is **undefined behaviour**.
  - In `3staticcast.cpp`, a `B` object read through a `C*` still calls
    `C::dump()`, because non-virtual calls are bound at compile time. That
    function reads `B`'s memory at `C`'s offsets, so it prints garbage or
    the wrong fields. A virtual `dump()` there could crash.

### Naming-service problem
- `PROBLEM`: register `(name, Object*)` tuples for objects of many classes,
  then get them back and use them through the correct cast, flagging and
  skipping the wrong ones.
- Solution (`problemsolve.cpp`): a polymorphic `Object` base with a virtual
  destructor, `unordered_map<string, Object*>` as the registry, and
  `dynamic_cast` plus a null check on lookup.

### Copy elision (`copyElision.txt`)
- The compiler builds the object **directly in its final location** and
  skips the temporary and the copy/move.
- This means **side effects inside copy/move constructors may not run**.
  Don't rely on them.
- **RVO**: when you return an unnamed temporary (`return Widget();`). Since
  C++17 this is **guaranteed**.
- **NRVO**: when you return a named local (`return localWidget;`). This is
  allowed but **not guaranteed**.

## Exam cheat-sheet

| Question | Answer |
|---|---|
| `A b = a;` calls? | Copy constructor (new object) |
| `b = a;` calls? | Copy assignment (existing object) |
| Default copy with pointer members? | Shallow: shared memory, risk of double delete or leak |
| Need custom copy ctor / `operator=` / destructor? | When the class owns a resource (Rule of Three) |
| Base class used polymorphically? | Make the destructor `virtual` |
| Safe downcast? | `dynamic_cast` + null check (needs a virtual function) |
| `static_cast` downcast to the wrong type? | Undefined behaviour, no error reported |
| RVO vs NRVO? | Unnamed temp (guaranteed since C++17) vs named local (optional) |

## Build

```bash
g++ -std=c++17 virdest.cpp && ./a.out
echo 0 | ./a.out   # 2dynamic / 2.1statc / 3staticcast read a choice: 0 = B, else C
```
