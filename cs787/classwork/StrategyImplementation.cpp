#include <bits/stdc++.h>
using namespace std;


class Compositor {
    public:
        virtual int Compose(
            Coord natural[], Coord stretch[], Coord shrink[],
            int componentCount, int lineWidth, int breaks[]
        ) = 0;
    protected:
        Compositor() = default;
}

class Composition {
    public:
        Composition(Compositor*);
        void Repair();

    private:
        Compositor* _compositor;
        Component* _components; // List of components
        int _componentCount;    // The number of components
        int _lineWidth; // the Composition's line width
    int* _lineBreaks;   // position of line breaks in components.
    int _lineCount;     // number of lines
};

void Composition::Repair() {
    Coord* natural;
    Coord* stretchability;
    Coord* shrinkability;
    int componentCount;
    int*   breaks;

    // prepare the arrays with the desire component sizes
    // ...

    // determines where the breaks are:
    int breakCount;
    breakCount = _compositor->Compose(
        natural, stretchability, shrinkability,
        componentCount, _lineWidth, breaks
    );

    // layout components according to the breaks.
}

class SimpleCompositor : public Compositor {
    public:
        SimpleCompositor();
        virtual int Compose(
            Coord natural[], Coord stretch[], Coord shrink[],
            int componentCount, int lineWidth, int breaks[]
        );
        // ...
};

class TexCompositor : public Compositor {
    public:
        TexCompositor();
        virtual int Compose(
            Coord natural[], Coord stretch[], Coord shrink[],
            int componentCount, int lineWidth, int breaks[]
        );
        // ...
};


class ArrayCompositor : public Compositor {
    public:
        ArrayCompositor(int);
        virtual int Compose(
            Coord natural[], Coord stretch[], Coord shrink[],
            int componentCount, int lineWidth, int breaks[]
        );
        // ...
};

Composition* quick = new Composition(new SimpleCompositor());
Composition* slick = new Composition(new TexCompositor());
Composition* iconic = new Composition(new ArrayCompositor(100));

