# 14th Oct 2026 — Namespaces & Argument Dependent Lookup (Koenig Lookup)

Quick recap of the lecture:
- why namespaces exist, and `using` to unlock them
- anonymous namespaces as a replacement for `static`
- the scope resolution operator `::`, and name conflicts between namespaces
- transitive `using`, nested namespaces, and aliasing
- Argument Dependent Lookup (ADL / Koenig lookup)

Most files are **"will this compile? which `x`?"** puzzles. The results
below come from actually compiling each one.

## Files

| Topic | Files | Result |
|---|---|---|
| Named namespace is locked until you write `using` | `0usingunlocks.cpp` | ❌ `p`, `q` not declared |
| Anonymous namespace is already unlocked in its file | `1anonymousnamespaces.cpp` | ✅ `30` |
| `using` one namespace doesn't unlock another; use `::` | `2scoperesolution.cpp` | ❌ `x` not declared (needs `A::x`) |
| Local variable vs `using` names | `3conflict-usescope.cpp` | ❌ redeclaring `x`; without that line the local `x` wins → `20` |
| Two `using`s that both bring in `x` | `3-1conflict-usescope.cpp` | ❌ `x` is ambiguous |
| One `using`, plus a reference through `H::x` | `3-2conflict-usescope.cpp` | ✅ `10 10` |
| Anonymous namespace vs `static` (internal linkage) | `4filescope-static-not-needed.cpp` + `h.cpp` | ✅ `101 102 1001` / `101 103 1002` |
| `using` inside a namespace is transitive | `5nestednamespaces.cpp` | ✅ `10 20` |
| Transitive `using` causing ambiguity | `6nestednamespaces.cpp` | ❌ `y` is ambiguous |
| Namespace aliasing | `7aliasing.cpp` | ✅ `10 10` |
| `using` scope, plus name hiding | `8question.cpp` | ✅ `10`, `25`, `20....` |
| ADL basics | `0.cpp`, `1.cpp`, `2.cpp`, `koenig1.cpp` | ✅ |
| ADL with overloads across namespaces | `koenig2.cpp` – `koenig5.cpp` | some calls ❌ (on purpose) |

Build: `g++ file.cpp && ./a.out`. For file 4 you need both files:
`g++ 4filescope-static-not-needed.cpp h.cpp`.

---

## Key concepts

### Namespaces and `using`
- A namespace groups names to avoid clashes between libraries and modules.
- Names in a **named** namespace are "locked". To use them, you need either
  `NS::name` or `using namespace NS;`.
- `using namespace B;` unlocks **only B**. Names in A still need `A::x`.
- **Scope resolution `::`**: `NS::x` always picks exactly that `x`. Use it
  whenever two names clash.

### Name conflicts (which `x`?)
- **A local variable hides names brought in by a `using` directive.** A
  local `int x = 20` beats `H::x` (`3conflict`).
- Declaring a second local `x` in the same block is a redeclaration error.
  This happens even when it is a reference: `int &x = H::x;`.
- Bringing in `x` from **two** namespaces (`using H; using K;`) is not an
  error by itself. It only becomes an **ambiguity error when you actually use
  `x`** (`3-1`). The fix is to qualify it: `H::x` or `K::x`.

### Anonymous (unnamed) namespaces
- `namespace { int p; }` is **automatically unlocked** inside its own file.
  You don't need `using`.
- The compiler treats it as a namespace with a unique hidden name, plus an
  automatic `using` for that name. No other file can ever name it.
- It gives **internal linkage** and **static storage duration**, which is
  the same effect as file-scope `static`.
- `h.cpp` can use `extern int z;` (normal global) but **cannot** reach `x`
  (`static`) or `y` (anonymous namespace).
- **Preferred over `static`**, because it also works for types: you can put
  a `class` or `struct` in an anonymous namespace, but `static` can't be
  applied to a type.

### `using` inside namespaces: transitive, not nested
- If `G` contains `using namespace H;`, then `using namespace G;` in `main`
  also brings in H's names (`5`).
- This is **not** nesting. A nested namespace is one physically defined
  inside another: `namespace A { namespace B { … } }`, used as `A::B::x`.
- Transitive `using` can create ambiguity. If both `G::y` and `H::y` become
  visible, `y` is ambiguous (`6`).

### Scope of a `using` directive (`8question.cpp`)
- A `using` directive only applies **in the block where it is written**. A
  `using` inside `g()` does nothing for `main`. With that change, `x` and
  `f` become undeclared in `main`.
- In `main`, the local `y = 25` hides `G::y` and `H::y`.
- Inside `G::f()`, `y` is `G::y = 20`. G's own member beats names it
  brought in through `using`.

### Namespace aliasing (`7aliasing.cpp`)
- `namespace A = H; namespace B = A;` gives H another name. Aliases can
  point to other aliases.
- Useful for shortening long or deeply nested names, for example
  `namespace fs = std::filesystem;`.

### Argument Dependent Lookup (ADL / Koenig lookup)
- In an **unqualified** call `f(arg)`, the compiler searches the normal
  scopes **and also the namespaces of the argument types**.
- `f(&a)` with `NS::A a` finds `NS::f`, even though there is no `using`
  (`1.cpp`, `2.cpp`).
- `std::cout << str` works because ADL finds `std::operator<<` for
  `std::string` (`0.cpp`).
- A function with **no arguments** gets no ADL, so it must be qualified:
  `A::f()` (`koenig1`).
- If there are several arguments, **every** argument's namespace is
  searched, in no particular order.
  - All the matches go into one overload set, and normal overload resolution
    picks the best match or reports an ambiguity.
  - Example: `mi(y, x)`, which takes a `B::Y` and an `A::X`, is found in `A`
    (`koenig3`).
- ADL only searches the argument's **own** namespace. Examples that fail:
  - `mip(y2)` with an `A::Y` doesn't find `B::mip`.
  - `g(x, y)` with a `B::Y` doesn't match `A::g(X, A::Y)`.
  - `g(A::Y)` doesn't exist in A, so there is no match (`koenig3`/`4`/`5`).
- Types with the same name in different namespaces (`A::Y` vs `B::Y`) are
  **different types**. Which namespace ADL searches depends on that.

## Exam cheat-sheet

| Question | Answer |
|---|---|
| Named vs anonymous namespace visibility in its own file? | Named: locked (needs `using` or `::`). Anonymous: unlocked |
| Anonymous namespace linkage? | Internal, like `static`, but also works for types |
| Local var vs `using`-imported name? | Local hides it |
| Same name from two `using`s? | Ambiguity error **only when used** |
| `using` inside a function? | Applies to that block only |
| `using` inside a namespace? | Transitive: comes along when that namespace is used |
| Alias syntax? | `namespace Short = Long::Name;` |
| ADL looks where? | Namespaces of the argument types (all of them, merged into one overload set) |
| No-arg call or qualified call? | No ADL |
