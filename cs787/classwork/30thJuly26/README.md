# 30th July 2026 — Design Patterns: Prototype & Strategy

Two GoF patterns. **Prototype** is a creational pattern: make new objects by
copying an existing one. **Strategy** is a behavioural pattern: swap
algorithms behind a common interface.

## Files

| File | What it is |
|---|---|
| `prototype_30thJuly2026.cpp` + `prototype_30thJuly26.png` | Prototype: `Shape` with `clone()`, `Circle`, `Square` (UML in the PNG) |
| `prototype_design_pattern.cpp` | GoF sample: `MazePrototypeFactory` (reference excerpt, doesn't compile) |
| `Strategy_30thJuly2026.cpp` + `Strategy_30thJuly.png` | Strategy: `TextProcessor` with `Capitalize`, `makeLower`, `Reverse` (UML in the PNG) |
| `StrategyImplementation.cpp` | GoF sample: `Composition` / `Compositor` line breaking (reference excerpt, doesn't compile) |

Build: `g++ prototype_30thJuly2026.cpp && ./a.out` (same for the Strategy
file).

---

## Prototype (GoF, creational)

> **Intent:** *Specify the kinds of objects to create using a prototypical
> instance, and create new objects by copying this prototype.*

| Role | In code |
|---|---|
| **Prototype**: declares `clone()` | `Shape` (`virtual Shape* clone() = 0`) |
| **ConcretePrototype**: implements `clone()` by copying itself | `Circle`, `Square` |
| **Client**: creates objects by asking a prototype to clone itself | `main()`, `MazePrototypeFactory` |

- **Key idea: a "virtual constructor".**
  - Constructors can't be virtual, so `new ???(*p)` can't pick the right
    class when you only have a `Shape*`.
  - `p->clone()` dispatches to the concrete class, so the copy has the
    correct dynamic type without the client knowing that type.
- **Demo output:**
  - `s2 = s1->clone(); s2->f();` grows only the copy:
    `Circle 10 / 15`, `Square 12 / 18`.
  - The original and the clone are independent objects.
- **GoF maze example:**
  - `MazePrototypeFactory` is configured with prototype objects
    (`Wall`, `Room`, `Door`).
  - Each `makeX()` clones its prototype, then initialises it (for example
    `setRoomNumber`, or `initialize` for doors).
  - Passing `BombedWall` / `RoomWithABomb` prototypes gives a different maze
    with **no new factory subclass**.
- **Use it when:**
  - the classes to create are chosen at runtime
  - you want to avoid a factory class hierarchy that mirrors the product
    classes
  - objects come in only a few configurations, so cloning a configured
    prototype is easier than building one from scratch
- **Consequences:**
  - ✅ Products can be added or removed at runtime, and fewer subclasses are
    needed.
  - ❌ Every class must implement `clone()`, and **deep vs shallow copy**
    has to be decided for each member, which is hard when objects contain
    pointers or cycles.
- **C++ notes:**
  - Implement `clone()` through the copy constructor:
    `return new Circle(*this);`. That copies *all* members. The demo's
    `new Circle(radius)` only works because each shape has just one field.
  - **Covariant return types** are allowed:
    `Circle* clone() const override`.
  - Modern style returns `std::unique_ptr<Shape>`. The demo's `new`
    objects are never deleted.

## Strategy (GoF, behavioural)

> **Intent:** *Define a family of algorithms, encapsulate each one, and make
> them interchangeable. Strategy lets the algorithm vary independently from
> clients that use it.*

| Role | In code |
|---|---|
| **Strategy**: common interface for all the algorithms | `TextProcessor::process()` · GoF: `Compositor::Compose()` |
| **ConcreteStrategy**: one algorithm each | `Capitalize`, `makeLower`, `Reverse` · GoF: `SimpleCompositor`, `TexCompositor`, `ArrayCompositor` |
| **Context**: holds a Strategy and delegates to it | GoF: `Composition` (holds a `Compositor*`, and `Repair()` calls `Compose()`) |

- **Demo output:** the same call, `tp->process("puShpEndra")`, gives
  `PUSHPENDRA`, then `pushpendra`, then `ardnEphSup`, depending on which
  strategy `tp` points to.
- **GoF example:**
  - A `Composition` (text) breaks lines using whichever `Compositor` it was
    given: simple, TeX-quality, or fixed-width array.
  - `quick`, `slick` and `iconic` are the same class with different
    strategies.
- **Use it when:**
  - several related classes differ only in behaviour
  - you need different variants of an algorithm
  - you want to replace a big `if`/`switch` that chooses an algorithm
  - you want to hide an algorithm's data from its clients
- **Consequences:**
  - ✅ Algorithms are reusable and can be swapped at runtime.
  - ✅ It removes conditionals and is an alternative to subclassing the
    context (Open/Closed principle).
  - ❌ Clients must know the strategies to pick one, there are more objects,
    and the interface has to be general enough for every algorithm (some
    may ignore some parameters).
- **Note:** `Strategy_30thJuly2026.cpp` has **no separate Context class**.
  `main` holds the `TextProcessor*` itself. In GoF, the context owns the
  strategy, so the client only configures it once.
- **Related:** State has the same structure, but the *object* switches its
  own state, whereas a Strategy is usually chosen by the client.
  `std::function` / lambdas are a lightweight Strategy in modern C++.

## GoF excerpts: typos to keep in mind
- They are copied from the book and are incomplete. `Maze`, `Room`, `Coord`
  and other types are never defined.
- `prototype_design_pattern.cpp`:
  - `intialize` is declared, but `initialize` is called.
  - `BombedWall` declares `Clone()` (capital C) returning `Wall*`, but
    defines it returning `BombedWall*`.
- `StrategyImplementation.cpp`: a `;` is missing after `class Compositor { … }`.
