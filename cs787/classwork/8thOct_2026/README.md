# 8th Oct 2026 — Templates, STL Containers, Range-for & Metaprogramming

Recap of the lecture:
- class templates with type and non-type parameters
- STL containers (`stack`, `queue`, `set`, `unordered_map`) and the
  reasoning behind their design
- range-`for` and the copies that happen inside it
- template metaprogramming: computing values at compile time

## Files

| Topic | File | Output / Result |
|---|---|---|
| Type template parameter | `typetemplates.cpp` | `TQ<int>`, `TQ<string>` from one class |
| Non-type template parameter | `nontypeparams.cpp` | `TArray<int, 10>`: squares 0..81 |
| Two type parameters, reverse lookup | `associative.cpp` | `0 0 1 table` |
| `std::stack`, wrapper with copy/move ctors | `stack.cpp` | `30 20 10`, then the final `pop()` aborts |
| `std::queue` | `queue.cpp` | `10 20 30` (FIFO) |
| Custom set: ∪ ∩ −, map/filter/reduce, concepts | `set.cpp` | `+: 10 20 30 50 100 200`, `-: 10 50`, `*: 30`, `reduce: 90` |
| Range-for over a vector | `rangefor-vector.cpp` | `10000000 200000 3000` |
| Range-for over an array | `rangefor-array.cpp` | `1000 2000 3000` |
| Range-for over a string | `rangefor-str.cpp` | size 5, capacity 15 |
| Metaprogramming: factorial | `metafact.cpp` | `Fact<5> = 120` |
| Metaprogramming: Fibonacci | `fiboMeta.cpp` | `Fib<5> = 8` (with `Fib<0> = Fib<1> = 1`) |
| Metaprogramming: Collatz (assignment) | `collatzSeq.cpp` | `Steps<5> = 6`, `Steps<3> = 8` |
| Notes: ADT reasoning, why `pop()` returns `void` | `reasoning.md` | — |

Build: `g++ -std=c++20 <file>.cpp && ./a.out`. `set.cpp` needs C++20 for
concepts.

---

## Templates

- **Type parameter (`typename T`):** one class definition, and the compiler
  generates a separate class for each `T` you use. `TQ<int>` and
  `TQ<string>` are unrelated types.
- **Non-type parameter (`int S`):** the value must be a **compile-time
  constant**, which is why `N` is `const`. Every different value creates a
  new type: `TArray<int,10>` is not the same type as `TArray<int,20>`.
- **Several parameters (`KEY, VAL`):** `associative.cpp` wraps an
  `unordered_map`. Its `operator[]` does a **reverse lookup** (value → key).
  - That lookup is an O(n) scan.
  - With duplicate values, the result depends on `unordered_map`'s order,
    which isn't defined.
  - If nothing matches, it returns `KEY()`.
- **Each type must support what the code does with it:**
  - `cout << T` for printing
  - `==` for comparisons
  - a hash for `unordered_map` keys
  - a default constructor for `KEY()` and `m[k]`
- **Return `*this` by reference** to allow chaining: `Q << a << b`,
  `a.add(..).add(..)`.
- **Wrapper / adapter classes** (`TQ`, `AssociativeArray`, `bigSet`) put a
  smaller interface over an STL container. `std::stack` and `std::queue` are
  built the same way.

## STL containers and the reasoning behind them (`reasoning.md`)

- **ADT thinking:** reason about a container through its **operations and
  their meaning**, not how it is built inside. A `stack` could sit on a
  deque, a vector or a list, and your code shouldn't care.
- **Why `std::stack::pop()` returns `void`** (exception safety):
  - If `T pop()` removed the element and then copying it into the return
    value threw an exception, the element would be lost.
  - So `top()` (look) and `pop()` (remove) are separate operations. This is
    command-query separation.
- **`pop()` on an empty `std::stack` is undefined behaviour, not an
  exception.**
  - The comment in `stack.cpp` says it throws, but on this machine it fails
    an internal library assertion (`!this->empty()`) and aborts.
  - `bigSet::pop()` in `set.cpp` does throw `out_of_range`. Compare the two.
- `set` stores **unique, sorted** elements, so inserting `10` twice keeps one
  copy. `queue` is FIFO and `stack` is LIFO.

### `set.cpp`: the `bigSet<T>` exercise
- Operators: `+` union, `-` difference, `*` intersection.
- Member templates `map(F)`, `filter(F)` and `reduce(init, F)` take lambdas.
  A `map` whose function sends two elements to the same result can make the
  set **shrink**.
- **Concept:** `concept Printable = requires(ostream& os, const T& v) { os << v; };`.
  - `print() requires Printable<T>` makes `print()` exist only for types
    that can be printed.
- **Copy elision in action:** the copy and move constructors print messages,
  but `bs1 + bs2` and the others print **only `bigSet constructor`**. The
  local `res` is built directly in place in the caller (NRVO), so no copy or
  move happens.

## Range-for and hidden copies

The copy constructor of `A` multiplies `x` by 100, so every copy shows up in
the output.

- **`rangefor-vector.cpp`:** `push_back` **reallocates** as capacity grows
  1 → 2 → 4.
  - Each time it moves to a new block, it copies all the old elements again.
  - `a1` was copied 3 times (`10×100³`), `a2` twice, and `a3` once, giving
    `10000000 200000 3000`.
  - GCC copies the new element **before** the old ones, which explains the
    odd print order.
  - It **copies rather than moves**, because writing a copy constructor
    suppresses the implicit move constructor. A move constructor would also
    have to be `noexcept` before `vector` used it during reallocation.
  - Fixes: `reserve(3)` gives 1 copy each. `emplace_back(10)` builds in place
    with no copy at all.
  - `push_back(const T&)` takes a reference, but it still has to
    **copy-construct** its own element. `emplace_back(Args&&...)`
    perfect-forwards **constructor arguments**.
- **`rangefor-array.cpp`:**
  - `A arr[3] = {a1, a2, a3}` is aggregate initialisation: exactly one copy
    per element, in left-to-right order, and no reallocation, giving
    `1000 2000 3000`.
  - `vector<A> v{a1, a2, a3}` would copy **twice** per element: once into the
    `initializer_list`, then again out of it.
- **`auto&` vs `auto` in the loop:** `for (auto x : c)` copies each element.
  Use `auto&` or `const auto&`.
- **`rangefor-str.cpp`:** a string can be iterated character by character.
  Its capacity of 15 for a 5-character string is libstdc++'s **small string
  optimisation**: short strings are stored inside the object, with no heap
  allocation.

## Template metaprogramming

- The compiler does the computation **during template instantiation**:
  - a recursive template is the recursive call
  - a full specialisation is the base case
  - the result is stored in `enum { val = ... }`
  - there is **zero runtime cost**
- **`metafact.cpp`:** `Fact<N> = N * Fact<N-1>`, with `Fact<0> = 1`.
- **`fiboMeta.cpp`:** `Fib<N> = Fib<N-1> + Fib<N-2>`, with two base cases.
  - Each `Fib<N>` is instantiated only once, so the compiler effectively
    remembers earlier results.
  - C++20 warns about doing arithmetic between two *different* enum types.
    `static constexpr int val = ...;` is the modern form.
- **`collatzSeq.cpp` (assignment):** `Steps<N>` counts the terms of the
  Collatz sequence (`N/2` if `N` is even, `3N+1` if odd) down to 1.
  - `if` can't go inside a struct body, so the decision is made with a
    **`bool` template parameter**: `NextStep<N, N%2==0>` selects one of two
    partial specialisations. Only the branch actually taken is instantiated.
  - A ternary `N%2==0 ? Steps<N/2>::val : Steps<3N+1>::val` **fails**. It
    instantiates **both** branches, and the untaken paths eventually loop
    back (`Steps<10>` → `Steps<5>`, which is still incomplete) or grow
    forever.
  - **Lesson:** choosing a branch at compile time must happen through
    specialisation (or `std::conditional_t`), never by computing both and
    picking one.

## Quick recap
1. Templates generate types. Each type must support the operations the
   template uses.
2. Non-type template arguments must be compile-time constants.
3. STL APIs are designed for exception safety: `top()` and `pop()` are
   separate.
4. `vector` growth copies elements. Use `reserve`, `emplace_back`, and a
   `noexcept` move constructor.
5. Range-for with `auto&` avoids copies.
6. Metaprogramming = recursion through templates + specialisation as the
   base case. Branches must be lazy.
