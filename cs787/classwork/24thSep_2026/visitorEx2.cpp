#include <bits/stdc++.h>
using namespace std;


class Visitor;

// 1) Element
class Element {
    public:
        virtual void accept(Visitor& v) = 0;
        virtual ~Element() = default;
};

class Circle : public Element {
    public: 
        void accept(Visitor& v) override {
            v.visit(*this);
        }
};


// 2) Concrete Element
class Rectangle : public Element {
    public:
        void accept(Visitor& v) override {
            v.visit(*this);
        }
};

class Triangle : public Element {
    public:
        void accept(Visitor&v) override {
            v.visit(*this);
        }
};


// 3) Visitor
class Visitor {
    public:
        virtual void visit(Circle&) = 0;
        virtual void visit(Rectangle&) = 0;
        virtual void visit(Triangle&) = 0;
};

// 4) ConcreteVisitor
class DrawVisitor : public Visitor {
    public:
        void visit(Circle& c) override {
            cout << "Drawing Circle\n";
        }
        void visit(Rectangle& r) override {
            cout << "Drawing Rectangle\n";
        }
        void visit(Triangle& t) override {
            cout << "Drawing Triangle\n";
        }
};

class AreaVisitor : public Visitor {
    public:
        void visit(Circle& c) override {
            cout << "Area of Circle\n";
        }
        void visit(Rectangle& r) override {
            cout << "Area of Rectangle\n";
        }
        void visit(Triangle& t) override {
            cout << "Area of Triangle\n";
        }
};

class Drawing {
    vector<Element*> elements;

    public:
        void add(Element *e){
            elements.push_back(e);
        }
        void accept(Visitor& v) {
            for(auto e: elements){
                e->accept(v);
            }
        }

        ~Drawing() {
            for(auto e : elements){
                delete e;
            }
        }
};


class Document {
    vector<Element*> elements;

    public:
        void add(Element* e) {
            elements.push_back(e);
        }

        void accept(Visitor &v){
            for(Element* e : elements) {
                e->accept(v);
            }
        }
};


int main() {
    Document doc;
    doc.add(new Circle());
    doc.add(new Rectangle());
    doc.add(new Triangle());

    DrawVisitor draw;
    doc.accept(draw);
}
