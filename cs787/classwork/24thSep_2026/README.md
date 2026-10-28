# 24th Sep 2026 — Design Pattern: Visitor

Quick recap of the lecture on the **Visitor** pattern (GoF, behavioural).
It separates **operations** from the **objects they operate on**, and uses
**double dispatch** to pick the right code for each element type.

## Files

| File | What it has |
|---|---|
| `DesignPattern_Visitor.pdf` | Full revision sheet: problem, participants, double dispatch, AST use case, pros/cons, trade-off |
| `visitor.md` | ASCII diagrams of the Visitor and Element hierarchies and the ObjectStructure |
| `visitorEx1.cpp` | Minimal version: `Shape` → `Circle`, `Rectangle`, plus one `DrawVisitor` |
| `visitorEx2.cpp` | All 5 GoF participants: adds `Triangle`, `AreaVisitor`, and `Document`/`Drawing` as the ObjectStructure |

`visitorEx1.cpp` output: `Drawing Circle` / `Drawing Rectangle`.

> ⚠️ `visitorEx2.cpp` **does not compile as written** ("invalid use of
> incomplete type 'class Visitor'"). Each `accept()` calls `v.visit()` while
> `Visitor` is only forward-declared. To fix it, either:
> - move the `Visitor` class above the elements (after forward-declaring
>   `Circle`, `Rectangle` and `Triangle`), or
> - define the `accept()` bodies after `Visitor`.
>
> With the fix it prints `Drawing Circle` / `Drawing Rectangle` /
> `Drawing Triangle`.

---

## Key concepts

### The problem
- You have a **stable** set of element classes, for example
  `Circle`, `Rectangle`, `Triangle` or compiler AST nodes.
- You keep adding **new operations** on them: draw, area, export, type-check,
  TAC generation, and so on.
- If you put every operation inside every class, the classes fill up with
  unrelated code and must be edited for each new operation.

### The idea
- Each operation becomes its own **Visitor** class. The elements only need
  an `accept()` hook.
- **Stable objects + changing operations ⇒ Visitor.**

### The 5 participants
| Participant | Role | In code |
|---|---|---|
| **Visitor** | Interface with one `visit(ConcreteElement&)` for each element type | `Visitor` |
| **ConcreteVisitor** | One complete operation | `DrawVisitor`, `AreaVisitor` |
| **Element** | Declares `accept(Visitor&)` | `Element` / `Shape` |
| **ConcreteElement** | Implements `accept()` as `v.visit(*this);` | `Circle`, `Rectangle`, `Triangle` |
| **ObjectStructure** | Holds and traverses the elements (decides *which* and *in what order*) | `Document`, `Drawing` |

### Why `accept()` is needed
- `visitor.visit(*basePtr)` doesn't work. Overload resolution uses the
  **static** type (`Element&`), not the runtime type.
- `basePtr->accept(visitor)` is a **virtual** call, so it reaches
  `Circle::accept()`.
- Inside `Circle::accept()`, `*this` is a `Circle&`, so `v.visit(*this)`
  picks `visit(Circle&)`.

### Double dispatch
- **1st dispatch:** the virtual `accept()` finds the concrete element.
- **2nd dispatch:** the virtual call to the overloaded `visit()` finds the
  concrete visitor and its matching overload.
- The final code depends on **both** the element type and the visitor type.
  C++ has no built-in double dispatch, so this pattern builds it.
- Call chain:
  `element->accept(v)` → `Circle::accept()` → `v.visit(*this)` → `DrawVisitor::visit(Circle&)`

### ObjectStructure vs Visitor
- ObjectStructure decides **where and in what order** (list, tree,
  Composite, AST).
- Visitor decides **what to do** at each element.
- Classic fit: a **compiler AST**. The node types are fixed, and many passes
  (type checking, pretty print, TAC generation, optimisation, codegen) are
  written as separate visitors.

### Trade-off (the most important point)
| Change | With Visitor |
|---|---|
| Add a new **operation** | ✅ Easy: write a new ConcreteVisitor, no element changes |
| Modify an existing operation | ✅ Localised to one visitor |
| Add a new **element type** | ❌ Hard: add a new `visit()` to the interface **and** to every ConcreteVisitor |

- **Open/Closed nuance:** Visitor is open for extension along the
  **operation** axis only. It deliberately makes the element hierarchy
  harder to extend.

### Pros / Cons
- **Pros:**
  - new operations are easy to add
  - all of one operation's logic stays in one place
  - algorithms are separate from the data
  - a visitor can **accumulate state** while it traverses
  - it handles mixed collections of element types well
- **Cons:**
  - adding element types is hard
  - it can weaken encapsulation, because visitors may need the elements'
    internals
  - visitors are tightly coupled to the concrete element types
  - the interface gets large when there are many elements
  - it is more complex than plain polymorphism

### Good practice seen in the code
- A virtual destructor in `Element`, because elements are deleted through a
  base pointer.
- The ObjectStructure (`Drawing`) owns the elements and deletes them in its
  destructor.
  - `Document` in `visitorEx2.cpp` doesn't do this, so it leaks.
  - `visitorEx1.cpp` deletes through `Shape*`, but `Shape` has no virtual
    destructor.

## Revision in 6 lines
1. Visitor separates operations from the object structure.
2. Each Element provides `accept(Visitor&)`.
3. Each ConcreteElement calls `visitor.visit(*this)`.
4. This gives **double dispatch**.
5. Adding operations is easy.
6. Adding element types is hard.

## Build
```bash
g++ visitorEx1.cpp && ./a.out
```
