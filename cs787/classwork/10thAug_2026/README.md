# 10th Aug 2026 — Design Pattern: Composite

## What it is (GoF)

> **Intent:** *Compose objects into tree structures to represent part–whole
> hierarchies. Composite lets clients treat individual objects and
> compositions of objects uniformly.*
>
> — *Design Patterns* (Gamma, Helm, Johnson, Vlissides), structural pattern

The **Composite** pattern is useful whenever something is made of parts, and
those parts can themselves be made of parts. For example:
- a drawing made of shapes and groups of shapes
- a GUI window made of panels, buttons and text boxes
- a file system made of files and folders
- an expression tree in a compiler

The client shouldn't have to ask *"is this a single item or a group?"*. It
calls `draw()` and the tree takes care of the rest.

## Participants

| Role | Responsibility | In `composites.h` |
|---|---|---|
| **Component** | Common interface for leaves and composites. Declares the operations (and optionally the child-management methods) | `IGraphic` |
| **Leaf** | A basic object with no children. Does the actual work | `Pen`, `Brush`, `Eraser`, `Shape` |
| **Composite** | Holds child `Component`s. Implements each operation by **passing it to every child**, optionally adding its own work before or after | `Dialog` (`vector<IGraphic*>`) |
| **Client** | Works with everything through the `Component` interface | `main()` |

```
              Component (IGraphic)
              + draw()  + move()  + copy()
                 ▲                 ▲
          ┌──────┘                 └──────┐
        Leaf                         Composite (Dialog)
   (Pen, Brush, …)                   children: [Component*]  ◇──┐
   draw(): do it                     draw(): for each child    │
                                              child->draw()  ──┘
```

Because `Dialog` is **itself** an `IGraphic`, a dialog can contain other
dialogs. That self-reference is what makes the structure a tree of any
depth.

## How it works
- **Recursive composition:** an operation on the root moves down the tree.
  - The composite calls the same operation on each child.
  - A child that is a composite does the same, until the calls reach the
    leaves.
- **Uniform treatment:** the client code is the same for one `Pen` and for a
  whole nested `Dialog`.
- **Typical recursive operations:** draw or render, move, size or price
  totals, search, **clone** (deep copy), and **equality** (compare the two
  trees one node at a time).

## Design decisions GoF discusses
- **Where child-management methods (`add`/`remove`) go:**
  - **Transparency:** declare them in Component. The client treats all nodes
    the same, but calling `add()` on a leaf makes no sense.
  - **Safety:** declare them only in Composite. Misuse is caught at compile
    time, but the client sometimes has to downcast.
  - The code here chooses **safety**: `addGraphic` is only on `Dialog`.
- **Ownership:** the composite usually **owns** its children and deletes
  them in its destructor. This is why Component needs a **virtual
  destructor**.
- **Child ordering:** keep the order when it matters, for example z-order
  when drawing.
- **Parent references:** optional. They make it easy to walk up the tree.
- **Caching:** a composite may cache aggregated results, such as a bounding
  box, and invalidate the cache when its children change.

## Consequences
- ✅ The client code is simple: one interface, no type checks.
- ✅ New kinds of Leaf or Composite work with existing client code (Open/Closed).
- ✅ Recursive structures of any depth come for free.
- ❌ The design can become too general. It's hard to limit *which*
  components a composite may contain, for example "a Dialog holds only
  Pens". That needs runtime checks.
- ❌ In the transparent version, leaves inherit child operations that make
  no sense for them.

## Related patterns
- **Iterator:** to traverse the tree.
- **Visitor** (24th Sep): adds new operations over the tree without
  changing the node classes.
- **Prototype:** `copy()` is a polymorphic clone, and copying a composite
  deep-copies its subtree.
- **Decorator:** has a similar structure (a component wrapping a
  component), but adds responsibilities rather than combining many parts.
- **Chain of Responsibility:** often uses the parent links.

## The code in this folder (`composites.h` / `composites.cpp`)
- The tree is `dialog1 → [dialog2 → [Pen, Brush], Eraser, dialog3 → [Shape, Pen]]`.
- `draw()` indents each level by 4 spaces, which shows the tree shape.
  `move()` and `copy()` go through the tree the same way.
- **Deep vs shallow copy:**
  - `copy()` clones every child recursively.
  - `shallowCopy()` copies only the child *pointers*, so both dialogs share
    the same children. Deleting both dialogs would delete the children twice
    (double delete).
- `operator==` compares two trees **structurally**:
  - same types (checked with `dynamic_cast`), same values, same order
  - so both copies compare equal
  - the program prints "same instance", but that is equality, not identity
    (it doesn't check that they are the same object)

Build: `g++ composites.cpp && ./a.out`
