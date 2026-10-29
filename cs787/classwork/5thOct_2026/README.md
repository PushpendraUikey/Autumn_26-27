# 5th Oct 2026 — Design Pattern: State

The **State** pattern (GoF, behavioural): an object **changes its behaviour
when its internal state changes**. It looks as if the object changed its
class. Instead of `if/else` or `switch` on a state flag inside every method,
each state is a class, and the object hands each call to its current state
object.

## File

`state.cpp`: a bounded **LIFO** (stack) of capacity 4, with three states:
`EMPTY`, `PARTIAL`, `FULL`.

```
g++ state.cpp && ./a.out
```

| Call | State before | Output | State after |
|---|---|---|---|
| `fetch()` | EMPTY | `Buffer is empty`, `-1` | EMPTY |
| `add(1)`, `add(2)`, `add(3)` | EMPTY → PARTIAL | `1 2 3` | PARTIAL |
| `fetch()` | PARTIAL | `3` | PARTIAL |
| `add(4)`, `add(5)` | PARTIAL | `4 5` | FULL (size 4) |
| `add(6)` | FULL | `Buffer is FUll`, `-1` | FULL |

## Participants

| Role | In code | Job |
|---|---|---|
| **Context** | `LIFO` | Holds the data (`buffer`) and a pointer to the `current` state. Passes `add`/`fetch` on to it |
| **State** (interface) | `LIFOState` | Declares the state-dependent operations: `add(x, ctx)`, `fetch(ctx)` |
| **ConcreteStates** | `EMPTY`, `PARTIAL`, `FULL` | Each implements the behaviour for one state. EMPTY rejects `fetch`, FULL rejects `add` |

## How it works
1. `LIFO::add()` / `fetch()` call `current->add(x, this)`. This is ordinary
   **virtual dispatch** on the current state object.
2. The state object works on the context's data. It receives a `LIFO*`, and
   the state classes are `friend`s, so they can reach the private `buffer`.
3. After each operation, `changeState()` moves to the next state based on
   the buffer's size: empty → EMPTY, at capacity → FULL, otherwise PARTIAL.
4. The three state objects are created **once** in the constructor and
   reused. Only the `current` pointer changes.

## Why use it
- It **removes state conditionals** from the context. Each state's rules live
  in one class.
- **Adding a state** means adding a new class plus a transition, with no
  edits scattered through every method. This follows the Open/Closed
  principle.
- The transitions are explicit and easy to follow.
- **Cost:** more classes, and the states are tightly coupled to the context
  (here through `friend`).

**State vs Strategy:** the class structure is the same (the context holds a
polymorphic object). In Strategy, the *client* picks the algorithm and it
rarely changes. In State, the *object itself* switches between states as it
runs.

## Things to note in the code
- **Who decides transitions:** here the **context** decides them
  (`changeState()`). The other common variant has each **state** set the
  next one, for example `lifo->current = lifo->fullstate`.
- **`FULL` is detected with `buffer.capacity()`.** That's fragile:
  `reserve(cap)` guarantees *at least* `cap`, not exactly `cap`. Storing
  `cap` in a member is safer.
- **Nothing is ever deleted.** `LIFO` has no destructor, so the three state
  objects leak, and so does `bb` in `main`. `LIFOState` does correctly have
  a virtual destructor.
- **Returning `-1` as the error value** clashes with a real `-1` element. An
  exception or `std::optional<int>` would be cleaner.
