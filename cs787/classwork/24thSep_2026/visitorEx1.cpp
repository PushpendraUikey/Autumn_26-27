#include <bits/stdc++.h>
using namespace std;

class Circle;
class Rectangle;

class Visitor {
    public:
        virtual void visit(Circle& c) = 0;
        virtual void visit(Rectangle& r) = 0;
};

class Shape {
    public:
        virtual void accept(Visitor& v) = 0;
};

class Circle : public Shape {
    public:
        void accept(Visitor& v) override {
            v.visit(*this);
        }
};

class Rectangle : public Shape {
    public:
        void accept(Visitor& v) override {
            v.visit(*this);
        }
};


class DrawVisitor : public Visitor {
    public:
        void visit(Circle& c) override {
            cout << "Drawing Circle" << endl;
        }
        void visit(Rectangle& r) override {
            cout << "Drawing Rectangle" << endl;
        }
};

int main() {
    vector<Shape*> shapes;
    shapes.push_back(new Circle());
    shapes.push_back(new Rectangle());

    DrawVisitor v;

    for(auto s : shapes){
        s->accept(v);
    }

    for(auto s : shapes){
        delete s;
    }
    return 0;
}