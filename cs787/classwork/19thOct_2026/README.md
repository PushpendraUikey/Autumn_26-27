# 19th Oct 2026 — Modern C++: Lambdas, Callables & Type Traits

The lecture had two halves:

1. **Lambda expressions**: what they are, how to store them, how to pass them
   to functions, generic (`auto`) lambdas, and using them to build the
   functional-programming trio **map / filter / reduce**.
2. **Type traits** (`<type_traits>`): asking the compiler questions about
   types at compile time, how `is_integral` is actually built, and using a
   trait to constrain a template with a C++20 `requires` clause.

## Files at a glance

| File | Topic | Output |
|---|---|---|
| `lambda1.cpp` | Defining and immediately calling anonymous lambdas | `30` / `even` / `odd` |
| `lambda2.cpp` | Lambda passed to `std::for_each` | squares of 0..9 |
| `lambda3.cpp` | Storing a lambda in an `auto` variable; generic `auto` params | `8`, `5.6`, `Hello World` |
| `lambda4.cpp` | Passing a lambda through a template parameter `F` | `Template Result: 50` |
| `lambda5.cpp` | Passing a lambda as `std::function<int(int,int)>` | `std::function Result: 5` |
| `stdfunc.cpp` | `std::function` wrapping an ordinary function | `Hello, World!` |
| `using-as-type-alias.cpp` | `using` to name a function-pointer type | `8` |
| `genericlambdas.cpp` | Generic lambda working on `int` and on a user class with overloaded `+` | `50` / `600` |
| `mapexercise.cpp` | Exercise: write `map` | `1 4 9 16 25` / `11 12 13 14 15` |
| `filter.cpp` | Exercise: write `filter` | `Even numbers: 2 4 6 8 10` |
| `reduceexercise.cpp` | Exercise: write `reduce` | `Sum: 15` / `Product: 120` |
| `1.cpp`, `2.cpp`, `3.cpp` | `is_integral`, `is_floating_point`, `is_class` on four types | `1000`, `0010`, `0101` |
| `usetrait.cpp` | `requires std::is_integral_v<T>` to restrict a template | prints 4 integral values |
| `type_trait.md` | Notes: how traits are implemented, the 4 categories of traits | — |

---

## Part 1 — Lambda expressions

### 1.1 Anatomy of a lambda

```cpp
[capture] (params) -> return_type { body }
```

- `[]`: the **capture clause**. It says which variables from the
  surrounding scope the lambda can use. All of today's examples use an empty
  capture `[]`, so they rely only on their parameters.
- `(params)`: an ordinary parameter list. Since C++14 these can be `auto`.
- `-> return_type`: optional. If you leave it out, the compiler works out the
  return type from the `return` statement.
- A lambda is an **anonymous function object**. The compiler generates an
  unnamed class with an `operator()`, and the lambda expression creates an
  object of that class. Because the type is known at compile time, calls are
  usually **inlined**. The comment in `lambda2.cpp` says this: *"inlined
  functions which are anonymous functions"*.

### 1.2 Defining and calling in place (`lambda1.cpp`)

```cpp
cout << [] (int x, int y) -> int { return x + y; } (10, 20) << endl;   // 30
[] (int x) { if (x & 1) cout << "odd"; else cout << "even"; } (10);    // even
```

A lambda is an expression, so you can call it straight away by putting
`(args)` after it. `x & 1` checks the lowest bit, which is a cheap odd/even
test.

### 1.3 Lambdas with STL algorithms (`lambda2.cpp`)

```cpp
for_each(v.begin(), v.end(), [](int x) { cout << x * x << endl; });
```

This is the most common use of lambdas: a short, one-off piece of behaviour
handed to an algorithm, with no need for a named helper function.

### 1.4 Storing a lambda in a variable (`lambda3.cpp`)

```cpp
auto add = [](auto a, auto b) -> auto { return a + b; };
add(5, 3);                                         // 8
add(2.5, 3.1);                                     // 5.6
add(std::string("Hello "), std::string("World"));  // Hello World
```

- You **must** use `auto` for the variable, because the lambda's type is a
  unique unnamed type that only the compiler knows.
- With `auto` parameters this is a **generic lambda**: one definition works
  for `int`, `double` and `std::string`. Behind the scenes, `operator()` is a
  template that gets instantiated once for each set of argument types.

### 1.5 Generic lambdas are polymorphic (`genericlambdas.cpp`)

```cpp
class C { public: int v; C(int i) { v = i; } };
int operator + (C x, C y) { return x.v * y.v; }   // note: '+' multiplies!

auto f = [] (auto x, auto y) { return x + y; };

f(20, 30);   // 50   -> built-in int +
f(c1, c2);   // 600  -> user-defined operator+ (20 * 30)
```

The file's comment says *"no types needed in lambda expressions, it works
polymorphic"*. `f` does not know or care what `x + y` means. It uses whatever
`+` exists for the argument types. Here `operator+` was deliberately written
to **multiply**, which shows that the lambda really calls the user's overload
and not a built-in addition. This is **compile-time (static) polymorphism**,
the same idea as templates. There are no virtual functions involved.

### 1.6 Three ways to pass a callable into a function

| Approach | File | Signature | Notes |
|---|---|---|---|
| Template parameter | `lambda4.cpp` | `template <typename F> void executeOperation(F operation)` | The compiler knows the exact lambda type, so the call can be inlined at **zero overhead**. Accepts any callable. |
| `std::function` | `lambda5.cpp`, `stdfunc.cpp` | `void executeOperationWithFunction(std::function<int(int,int)> op)` | The exact signature is written out. It is a **general-purpose polymorphic function wrapper** that can hold a lambda, a free function, a functor and so on. It uses type erasure, so there is a small runtime cost (indirect call, possible heap allocation), but it can be stored in containers and members and used in non-template code. |
| Function pointer + `using` alias | `using-as-type-alias.cpp` | `using MathOperation = int(*)(int, int);` | The old C-style way. `using` makes the type readable. It only works for plain functions and for lambdas with **no captures**, because those convert to a function pointer. |

```cpp
// stdfunc.cpp: std::function holding an ordinary function
std::function<void()> func = greet;
func();                                 // Hello, World!

// using-as-type-alias.cpp
using MathOperation = int(*)(int, int); // easier to read than typedef int (*MathOperation)(int,int);
MathOperation op = add;
op(5, 3);                               // 8
```

Rule of thumb: **use a template parameter** when the callable is only used
inside the call. **Use `std::function`** when you need to store it or keep
the function out of a header and non-templated.

---

## Part 2 — Functional programming exercises: map / filter / reduce

All three exercises have the same shape: a generic function that takes a
`vector<T>` and a lambda, where the lambda supplies the behaviour.

### 2.1 `map` (`mapexercise.cpp`)

> *map (vector of type T, a lambda expression) → produces another vector of
> type T by applying the expression to each value inside the vector*

```cpp
template <typename T>
vector<T> map (vector<T> v, auto f) { ... result.push_back(f(v[i])); ... }

template <typename T, typename F>
vector<T> map2 (vector<T> v, F f)    { ... same body ... }
```

- `map(v, [](int x){ return x * x; })`  →  `1 4 9 16 25`
- `map2(v, [](int x){ return x + 10; })` →  `11 12 13 14 15`

Both versions do the same thing. `map` uses the C++20 **abbreviated function
template** syntax (`auto` as a parameter type), which is shorthand for the
explicit `typename F` in `map2`.

### 2.2 `filter` (`filter.cpp`)

> *filter (vector of type T, lambda expression) produces a vector of type T
> by applying the given lambda expression, picking all values for which the
> expression returns true*

```cpp
template <typename T, typename F>
vector<T> filter(const vector<T>& vec, F func) {
    vector<T> result;
    for (const auto& item : vec)
        if (func(item)) result.push_back(item);
    return result;
}

filter(numbers, [](int x) { return x % 2 == 0; });   // 2 4 6 8 10
```

The lambda acts as a **predicate**, meaning it returns `bool`. This version
also takes the vector by `const&`, which avoids the copy that the `map` and
`reduce` versions make.

### 2.3 `reduce` (`reduceexercise.cpp`)

> *reduce (vector of a type, lambda expression) → collapses the vector of
> type T values to one value of type T*

```cpp
template <typename T, typename F>
T reduce (vector<T> v, F f, T init) {
    T result = init;
    for (...) result = f(result, v[i]);
    return result;
}

reduce(v, [](int x, int y){ return x + y; }, 0);   // Sum: 15
reduce(v, [](int x, int y){ return x * y; }, 1);   // Product: 120
```

The `init` value must be the **identity** of the operation: `0` for `+` and
`1` for `*`. If you seeded the product with `0`, the result would always be
`0`.

| | Input | Lambda shape | Output |
|---|---|---|---|
| map | `vector<T>` | `T → T` | `vector<T>` (same size) |
| filter | `vector<T>` | `T → bool` | `vector<T>` (same size or smaller) |
| reduce | `vector<T>` + init | `(T, T) → T` | single `T` |

The standard library already has these: `std::transform`, `std::copy_if`
and `std::accumulate` / `std::reduce`. Today's goal was to understand how
they work by writing them yourself with templates and lambdas.

---

## Part 3 — Type traits (`<type_traits>`)

### 3.1 What they are

Type traits do **compile-time introspection**. The compiler inspects, queries
or transforms properties of a type during compilation, so there is **zero
runtime cost**. A trait is a class template, and you read its answer through
`::value` (a boolean answer) or `::type` (a transformed type).

### 3.2 Experiments (`1.cpp`, `2.cpp`, `3.cpp`)

Each program asks the same question about `int`, `std::string`, `float` and
a user-defined empty `class A {}`, then prints the four bools in a row:

| Trait | `int` | `string` | `float` | `A` | Printed |
|---|:-:|:-:|:-:|:-:|---|
| `is_integral<T>` (`1.cpp`) | 1 | 0 | 0 | 0 | `1000` |
| `is_floating_point<T>` (`2.cpp`) | 0 | 0 | 1 | 0 | `0010` |
| `is_class<T>` (`3.cpp`) | 0 | 1 | 0 | 1 | `0101` |

The point to notice is that `std::string` counts as a class. It is a typedef
of `std::basic_string<char>`, so `is_class<string>` is true, the same as for
our own `A`.

### 3.3 How `is_integral` is built (from `type_trait.md`)

**Step 1: `integral_constant`.** This wraps a compile-time value inside a
type:

```cpp
template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept { return value; }
};

using true_type  = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;
```

**Step 2: a primary template that defaults to false, plus specializations
that say true.**

```cpp
template <typename T>
struct my_is_integral : false_type {};                 // default: "no"

template <> struct my_is_integral<bool>      : true_type {};
template <> struct my_is_integral<char>      : true_type {};
template <> struct my_is_integral<int>       : true_type {};
template <> struct my_is_integral<long>      : true_type {};
template <> struct my_is_integral<long long> : true_type {};
// ... and so on for every built-in integral type (signed/unsigned variants)

// Partial specializations to see through cv-qualifiers
template <typename T> struct my_is_integral<const T>          : my_is_integral<T> {};
template <typename T> struct my_is_integral<volatile T>       : my_is_integral<T> {};
template <typename T> struct my_is_integral<const volatile T> : my_is_integral<T> {};
```

For `my_is_integral<int>::value`, the compiler picks the `int`
specialization, which inherits `value = true`. For
`my_is_integral<double>::value`, no specialization matches, so the compiler
uses the primary template and gets `value = false`. The whole mechanism is
**template specialization plus inheritance**. No code runs at runtime.

### 3.4 The four major categories of traits

1. **Primary type categories**: *"What fundamental kind of type is T?"*
   `is_integral`, `is_floating_point`, `is_pointer`, `is_reference`,
   `is_array`, `is_class`, `is_enum`, `is_union`
2. **Composite type properties**: these combine primary categories.
   `is_arithmetic` (= integral || floating point), `is_scalar` (numbers,
   pointers, enums, pointers-to-members), `is_signed`, `is_unsigned`
3. **Type relationships**: how two types relate.
   `is_same<T, U>`, `is_base_of<Base, Derived>`, `is_convertible<From, To>`
4. **Type transformations**: these give a *new type* through `::type`
   instead of a bool.
   `remove_reference<T>` (`T&`/`T&&` → `T`), `remove_const<T>`,
   `add_pointer<T>` (`T` → `T*`), `decay<T>` (applies the same conversions as
   passing an argument by value: array → pointer, strips reference and
   `const`)

Shorthand helpers: `std::is_integral_v<T>` (C++17) means
`std::is_integral<T>::value`, and `std::remove_const_t<T>` (C++14) means
`typename std::remove_const<T>::type`.

### 3.5 Using a trait to constrain a template (`usetrait.cpp`)

```cpp
template <typename T>
requires std::is_integral_v<T>          // C++20 requires-clause
void print_number(T val) { std::cout << "Integral: " << val << "\n"; }

print_number(42);     // OK  -> Integral: 42
print_number('A');    // OK  -> Integral: A   (char is integral)
print_number(true);   // OK  -> Integral: 1   (bool is integral)
print_number(100L);   // OK  -> Integral: 100 (long is integral)
// print_number(3.14);   // error: constraints not satisfied
// print_number(2.718f); // error
// print_number("42");   // error (const char* is a pointer, not integral)
```

A **`requires` clause attaches a compile-time boolean condition** (a type
trait or a concept) to a template. If the condition is false, that template
is removed from overload resolution, and the error message names the failed
constraint directly. This is much clearer than the long errors an
unconstrained template produces when something goes wrong deep inside its
body. It is also the clean modern replacement for `std::enable_if` / SFINAE
tricks.

---

## Key takeaways

- A lambda is an **unnamed function object**. Store it with `auto`, call it
  immediately, or pass it to algorithms.
- **Generic lambdas** (`auto` params) give compile-time polymorphism. They
  work with any type that supports the operations used, including
  user-defined operator overloads.
- Pass callables through a **template parameter** for zero overhead, or
  through **`std::function`** when you need a fixed, storable type. Function
  pointers (named with `using`) only handle plain functions and lambdas with
  no captures.
- **map / filter / reduce** are just loops with the behaviour supplied by a
  lambda.
- **Type traits** answer questions about types at compile time, using
  `integral_constant`, `true_type`/`false_type` and template specialization.
- **`requires` + a trait** restricts what a template accepts and gives clear
  error messages.

## Build & run

Everything compiles with GCC in C++20 mode (needed for `requires` in
`usetrait.cpp` and the `auto` parameter in `mapexercise.cpp`):

```bash
g++ -std=c++20 usetrait.cpp -o usetrait && ./usetrait
# older GCC (pre-10) needed: g++ -std=c++17 -fconcepts usetrait.cpp
```

To see the constraint error, uncomment `print_number(3.14);` in
`usetrait.cpp`. GCC reports `no matching function for call to
'print_number(double)'` and points to the failed `is_integral_v<T>`
constraint.
