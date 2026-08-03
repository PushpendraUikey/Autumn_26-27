// ======================== DISCLAIMER ========================
// I correctly implemented the composition-based design, however the demonstration
// in main() and comments as well as documentation are written via "Claude AI".


// ============================================================================
// Assignment 2 — Combinatorics of Polymorphic Hierarchies
//
// PROBLEM
//   Given two independent polymorphic hierarchies:
//     Handler   with concrete subclasses H1, H2, H3   (size A = 3)
//     Transform with concrete subclasses G1, G2, G3   (size B = 3)
//   we want every Handler to be usable with every Transform, i.e. all
//   A * B = 9 combined behaviors, WITHOUT writing 9 separate classes
//   (H1G1, H1G2, ..., H3G3).
//
// DESIGN: composition instead of multiple inheritance (a "Bridge")
//   If we tried to get all combined behaviors purely through inheritance,
//   we would need one class per (Handler-kind, Transform-kind) pair -- the
//   class count grows as A*B, and adding one new Transform subclass would
//   force us to write A new classes (one per existing Handler).
//
//   Instead, each Handler HOLDS A POINTER to the abstract Transform base
//   (not to a specific subclass), set at runtime via assignTransform().
//   Because the pointer is typed as the *base* class Transform, it can be
//   bound to ANY subclass (G1, G2, G3, or a new G4 added later) without
//   Handler's code changing at all. This is exactly the "+P" arrow on the
//   board: Handler doesn't inherit from Transform, it merely *references*
//   it.
//
//   Consequences of this design:
//     - Only A + B = 6 classes need to be written, not A * B = 9.
//     - Any Handler can be paired with any Transform at RUNTIME, not just
//       at compile time -- rebinding is just calling assignTransform again.
//     - Adding a 4th Transform subclass (G4) requires writing ONE new
//       class; it immediately becomes usable with all existing Handlers
//       with zero changes to H1/H2/H3.
//     - Each hierarchy still contributes ITS OWN behavior: Handler
//       subclasses wrap pre/post logic around the call, Transform
//       subclasses supply the core transformation. The combined result is
//       a genuine function of *both* choices, which is what makes the 9
//       outputs below all different from each other.
// ============================================================================

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// ----------------------------------------------------------------------
// Hierarchy B: Transform (the "g()" family on the board)
// ----------------------------------------------------------------------
class Transform {
public:
    virtual ~Transform() = default;
    virtual int g(int x) const = 0;
    virtual std::string name() const = 0;
};

class G1 : public Transform {
public:
    int g(int x) const override { return x + 1; }
    std::string name() const override { return "G1(+1)"; }
};

class G2 : public Transform {
public:
    int g(int x) const override { return x * 2; }
    std::string name() const override { return "G2(*2)"; }
};

class G3 : public Transform {
public:
    int g(int x) const override { return x * x; }
    std::string name() const override { return "G3(sq)"; }
};

// ----------------------------------------------------------------------
// Hierarchy A: Handler (the "Handler" family on the board)
//   Holds a Transform* (assigned via assignTransform), and each concrete
//   Handler wraps the Transform's result with its OWN distinct logic --
//   this is what makes every (Handler, Transform) pair genuinely unique
//   rather than the Handler being a pure pass-through.
// ----------------------------------------------------------------------
class Handler {
public:
    virtual ~Handler() = default;

    void assignTransform(std::shared_ptr<Transform> t) {
        transform_ = std::move(t);
    }

    virtual int handle(int input) = 0;
    virtual std::string name() const = 0;

protected:
    std::shared_ptr<Transform> transform_;
};

class H1 : public Handler {
public:
    int handle(int input) override {
        int mid = transform_ ? transform_->g(input) : input;
        return mid + 100;                 // H1's own contribution: +100
    }
    std::string name() const override { return "H1(+100 after)"; }
};

class H2 : public Handler {
public:
    int handle(int input) override {
        int mid = transform_ ? transform_->g(input) : input;
        return -mid;                      // H2's own contribution: negate
    }
    std::string name() const override { return "H2(negate after)"; }
};

class H3 : public Handler {
public:
    int handle(int input) override {
        int pre = input * 2;              // H3's own contribution: double first
        return transform_ ? transform_->g(pre) : pre;
    }
    std::string name() const override { return "H3(*2 before)"; }
};

// ----------------------------------------------------------------------
// main: demonstrate ALL A*B = 3*3 = 9 combinations from just 6 classes.
// ----------------------------------------------------------------------
int main() {
    std::vector<std::shared_ptr<Handler>> handlers = {
        std::make_shared<H1>(),
        std::make_shared<H2>(),
        std::make_shared<H3>()
    };

    std::vector<std::shared_ptr<Transform>> transforms = {
        std::make_shared<G1>(),
        std::make_shared<G2>(),
        std::make_shared<G3>()
    };

    const int input = 5;

    std::cout << "Demonstrating " << handlers.size() << " x " << transforms.size()
              << " = " << handlers.size() * transforms.size()
              << " combined behaviors from only "
              << handlers.size() + transforms.size() << " classes\n";
    std::cout << "input = " << input << "\n\n";

    for (auto& h : handlers) {
        for (auto& t : transforms) {
            h->assignTransform(t);        // rebind at runtime -- no new class needed
            int result = h->handle(input);
            std::cout << h->name() << " x " << t->name()
                      << " -> " << result << '\n';
        }
    }

    return 0;
}