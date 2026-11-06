# 29th Oct 2026 — Variadic Templates, Fold Expressions & C++17 Vocabulary Types

Functions that take **any number of arguments of any types**: the old C way
(`va_list`), recursive variadic templates (C++11) and fold expressions
(C++17). Then three C++17 helper types (`std::variant`, `std::optional`,
`mutable`), all used together in a small Prolog-style logic engine.

## Files

| File | What it is |
|---|---|
| `examplevariadic.cpp` | Recursive variadic `print`: peel off the first argument, recurse on the rest, stop at the non-template `print()`. Prints `1 2 3 5 3 last` |
| `ex2variadic.cpp` | Same `print`, one value per line. A user class `A` also works because it has its own `operator<<` |
| `examplevariadic-notesAI` | Notes on how the recursion unrolls and the key points about it |
| `foldexpressions.cpp` | `sum1` = left fold `(... + args)`, `sum2` = right fold `(args + ...)`, `print` = comma fold. Prints `60`, `30`, `hello world how are you!` |
| `variadicfunction.cpp` | C-style variadics: `printf` calls plus `print(int count, ...)` built with `va_list` (`print(3, 10, 20, 30)` prints `10 20 30`). Its comments explain why C++ added variadic templates |
| `variant.cpp` / `variant.md` | `std::variant<int,double,string>`: `index`, `holds_alternative`, `get` (throws `bad_variant_access`), `get_if` (returns a pointer) |
| `optional.md` | `std::optional<T>`: a value that may be missing (`nullopt`), with no magic `-1` and no null pointers |
| `mutable.md` | `mutable` members: can change inside `const` functions. Bitwise vs logical constness |
| `VariadicChallange.cpp` | **Challenge:** Prolog-style facts, rules and queries built with variadic templates |

Build: `g++ -std=c++17 <file>.cpp && ./a.out`

---

## Key points

### Three ways to accept "any number of arguments"
| | C variadic (`va_list`) | Recursive template | Fold expression |
|---|---|---|---|
| Standard | C / C++98 | C++11 | C++17 |
| Type safe | ❌ you tell `va_arg` the type | ✅ | ✅ |
| Knows the count | ❌ passed by hand | ✅ `sizeof...(pack)` | ✅ |
| Base case | n/a | **required**, declared *before* the template | none |
| Work done | at runtime | expanded at compile time | expanded at compile time |

- **Pack expansion:** `typename... T` is a pack of types and `T... args` a
  pack of values. `args...` unpacks them back into a list.
- **One function per type list:** each different list of argument types
  creates its own function (`print<int,int>`, `print<int>`, ...).
  Identical lists reuse the same one.
- **Expanded at compile time, evaluated at runtime:** the compiler only
  *writes out* the expression. It is computed at compile time only in a
  `constexpr` context, or when the optimizer simplifies it.
- **By value vs by reference:** taking arguments by value copies them at
  every level, and turns `"hello"` into `const char*`. Prefer `const T&`.
- **`va_list` pitfalls:** reading the wrong type with `va_arg` is undefined
  behaviour (garbage values), and `char`/`float` are promoted to
  `int`/`double`.

### Fold expressions
- Where the `...` sits gives the direction:
  - `(... op p)` is a left fold: `((a op b) op c)`
  - `(p op ...)` is a right fold: `(a op (b op c))`
- The direction matters for operators like `-`: `(10-20)-30 = -40` vs
  `10-(20-30) = 20`.
- **Empty pack:** a unary fold over `+` fails to compile. Only `&&`, `||`
  and `,` allow an empty pack. A binary fold with a starting value fixes
  it: `(0 + ... + args)`.
- **Comma fold:** `((cout << args << " "), ...)` prints every argument with
  no recursion.
- **Not a fold:** `((cout << 10), 10)` is just the ordinary comma operator.
  The right side is evaluated and thrown away. A fold needs a pack **and**
  `...`.

### C++17 vocabulary types
- **`std::variant`:** a type-safe union. It always knows which type it
  holds, `std::visit` works like a type-safe `switch`, and no heap memory
  is used.
- **`std::optional`:** holds a `T` or nothing (`nullopt`). Like Haskell's
  `Maybe`.
- **`mutable`:** for logical constness, such as caches, counters or
  mutexes inside `const` objects.

### Challenge: a Prolog-style logic engine (`VariadicChallange.cpp`)
| Piece | How it works |
|---|---|
| `Fact(likes, john, mary)` | Variadic constructor. `args{&objs...}` stores the objects, an `&&` fold rejects variables, and the fact adds itself to `FactDB` |
| `Pattern` + `match()` | Arguments are `variant<Obj*, VARIABLE*>`. Matching (unification) needs the same predicate and arity. Then each position is checked: an object must be equal, an unbound variable gets bound, a bound variable must agree. Returns `optional<Bindings>` |
| `Find(likes, X, Y)` | Matches the pattern against every fact and fills each variable's `List`. The lists line up into answer tuples. Lists are `mutable` |
| `Rule(likes,X,Y, likes,Y,Z, likes,X,Z)` | The pack is flattened into a `variant` list and **split at every `Pred`**. The last pattern is the head. Backtracking matches the body, and **forward chaining** repeats until nothing new is added |

- **Result:** the rules derive `likes(john, jean)`, so
  `Find(likes, X, Y)` gives `X=[john, john]`, `Y=[mary, jean]`.
- **Gotchas:**
  - `likes/2` and `likes/3` are different relations.
  - Don't add facts to the database while looping over it: collect them
    first, then add.
  - Every variable in a rule's head must appear in its body.
