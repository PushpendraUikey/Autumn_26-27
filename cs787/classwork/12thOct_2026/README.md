# 12th Oct 2026 — Fail-Safe Programming: Exceptions & Assertions

Recap of the lecture:
- reporting and handling errors with **exceptions**
- what happens to objects while an exception is in flight
- exceptions in constructors and destructors
- `noexcept`
- **assertions** as checks that are only active in debug builds

## Files

| Topic | Files | Observed |
|---|---|---|
| `assert` and `NDEBUG` | `assert.cpp`, `assert-working.cpp` | With `NDEBUG`: no check (balance goes to `-190`). Without it: `Assertion 'bal > 100' failed`, abort |
| Throwing and catching by type | `exception.cpp` | `9800`, then the failed `account(10)` constructor throws a `char` and is caught |
| Catch-all `catch (...)`, exact type matching | `exception-default.cpp` | `int` thrown, not caught by `catch (char)` → `Default Exception handler` |
| `noexcept` | `exception1.2.cpp` | Compiler warns that the throw will always call terminate, then `terminate called…` (the `try`/`catch` doesn't help) |
| Catch by value vs by reference, slicing | `exception2.cpp`, `exception2_1.cpp`, `exception2_explanation.txt` | By value: extra copy plus extra destructor, and `C` sliced to `B`. By `const&`: no copy |
| Destructors must not throw | `exception7.cpp` | `removing engine` → `engine removal failed`, swallowed inside the destructor |
| Swallowing an exception in a constructor | `exception8.cpp`, `exception8.2.cpp` | Object "constructed" anyway → `makes sense?` (a zombie object) |
| `new` + throwing constructor | `exception8.1.cpp` | Pointer never assigned → `a->f()` prints garbage (UB) |
| Constructor function-try-block | `exception9.cpp`, `exception10.cpp` | Handler runs, then the exception is **rethrown automatically**. The destructor never runs |
| Handler translates the exception | `example10.1.cpp`, `example10.2.cpp` | `10.1`: `main` sees the new `logic_error`. `10.2`: `main` sees the original |
| Function-try-block on an ordinary function | `exception11.cpp` | Exception swallowed → `f returned normally` |
| Problem statement + LLM-written solutions | `fail-safe-bank-transfer.txt`, `money-transfer.cpp`, `ticket-booking.cpp` | — |

Build: `g++ <file>.cpp && ./a.out`. `exception2*.cpp` reads an input
(0 / 1 / 2).

---

## Key concepts

### Assertions (`<cassert>`)
- `assert(cond)` checks a condition **the programmer believes must always be
  true**. If it fails, the program prints the file, line and expression, then
  calls `abort()`.
- **`NDEBUG` removes every `assert` when the code is compiled**, so release
  builds pay nothing.
  - It must be defined **before** `#include <cassert>`, or passed on the
    command line with `-DNDEBUG`.
  - `assert-working.cpp` shows roughly what the `assert` macro expands to:
    `#ifndef NDEBUG … if (!cond) { report; abort(); } #endif`.
- **Assert vs exception:**
  - Use asserts for **bugs** (broken invariants, conditions that should be
    impossible). They are a debugging tool and are switched off in release
    builds.
  - Use exceptions for **expected runtime failures** (bad input, low
    balance, network errors), which must be handled in every build.
  - Never put code with side effects inside an `assert`, because it
    disappears when `NDEBUG` is defined.

### Exception basics
- `throw value;` → stack unwinding → the first matching `catch` handles it.
  Constructors have no return value, so **throwing is the only way to report
  a failed construction**.
- **Handlers are matched by type, with no ordinary conversions.** An `int`
  doesn't match `catch (char)`. Only these are allowed:
  - derived → base (by reference or pointer)
  - adding `const`
  - pointer conversions
- Handlers are tried **in order**:
  - Put `catch (...)` (catch-all) **last**. It catches anything but can't
    see the value.
  - A duplicate handler for the same type can never be reached, and the
    compiler warns about it.
- Prefer standard exception classes (`runtime_error`, `logic_error`, … in
  `<stdexcept>`, read with `e.what()`) over throwing raw `int`s or `char`s.

### Throw by value, catch by `const&`
- The thrown object lives in a special **exception space**, not on the
  thrower's stack. That's why it survives stack unwinding until the handler
  finishes.
- **Catch by value** makes a *copy*, so you see an extra destructor call.
  Catching a derived `C` as a `B` **slices** it to `B`.
- **`catch (const T& e)`** has no copy and no slicing, and virtual
  functions such as `what()` still work correctly.

### `noexcept`
- `noexcept` promises that a function won't throw. The compiler can
  optimise on that basis, and `vector` uses it to decide whether to move or
  copy when it reallocates.
- If an exception does escape, **`std::terminate()`** is called. No
  surrounding `try`/`catch` can stop it (`exception1.2.cpp`).

### Destructors and exceptions
- Destructors are **implicitly `noexcept`** (C++11 onwards). An exception
  that leaves one causes `terminate`.
- Throwing while the stack is already unwinding from another exception also
  causes `terminate`.
- **Rule:** a destructor must never let an exception escape. Wrap risky
  cleanup in a `try`/`catch` inside the destructor and log the error or set
  a failure flag (`exception7.cpp`).

### Constructors and exceptions
- **Never swallow a constructor failure.** If you do, the caller receives a
  half-built **"zombie" object** that looks valid but whose invariants are
  broken: uninitialised members, a `j` pointer that was never set
  (`exception8`, `8.2`).
- If the constructor throws:
  - The **destructor of that object doesn't run**, because the object never
    fully existed.
  - Members and bases that were already built are destroyed.
  - So use RAII members to clean up partial work.
- **`new` + a throwing constructor:** the memory is freed automatically, but
  the assignment `a = new A()` never happens. `a` stays uninitialised, and
  using it is undefined behaviour (`exception8.1`).
- **Function-try-block:** `A() try : members… { … } catch (…) { … }`.
  - It is the only way to catch exceptions thrown by **member or base
    initialisers**.
  - **For a constructor:** if the handler finishes normally, the exception is
    **rethrown automatically**, so the handler can't recover the object. It
    can only log, or translate by throwing a different exception (`10.1`).
    Don't use the object's members inside the handler, because they've
    already been destroyed.
  - **For an ordinary function:** there is no automatic rethrow, so the
    exception is swallowed (`exception11`). In a non-`void` function, falling
    off the end of the handler is undefined behaviour.

### Fail-safe programming
- Detect errors as early as possible. Report them in a way that **can't be
  ignored** (exceptions, not return codes that nobody checks).
- Never leave objects or the program in a half-updated state. Either
  complete the operation or restore the original state.
- Use asserts during development to catch logic bugs. Use exceptions in
  production to handle real failures.

## Quick recap
| Situation | Do |
|---|---|
| Programmer bug / broken invariant | `assert` (gone with `NDEBUG`) |
| Expected runtime failure | `throw` a standard exception |
| Catching | `catch (const T&)`, most specific type first, `catch (...)` last |
| Constructor can't finish | Let the exception escape. Never leave a zombie object |
| Destructor cleanup may fail | Catch inside the destructor. Never let it escape |
| Function promises not to throw | `noexcept` (a violation causes `terminate`) |
| Errors from member initialisers | Constructor function-try-block (log or translate, it rethrows anyway) |
