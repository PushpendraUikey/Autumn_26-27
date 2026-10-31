#include <bits/stdc++.h>
using namespace std;

class Shape {
    public:
        virtual ~Shape() = default;
        virtual Shape* clone() = 0;
        virtual void f() = 0;
        virtual string info() = 0;
};

class Circle : public Shape {
    public:
        Circle(int r) : radius(r) {}
        Shape* clone() override {
            return new Circle(this->radius);
        }
        void f() override {
            radius += (radius/2);
        }
        string info() override {
            return "Circle with radius: " + to_string(radius);
        }
    protected:
        int radius;
};

class Square : public Shape {
    public:
        Square(int s) : side(s) {}
        Shape* clone() override {
            return new Square(this->side);
        }
        void f() override {
            side += (side/2);
        }
        string info() override {
            return "Square with side: " + to_string(side);
        }
    protected:
        int side;
};

int main() {
    Shape *s1, *s2;

    s1 = new Circle(10);
    s2 = s1->clone();
    s2->f();
    cout << s1->info() << endl;
    cout << s2->info() << endl;

    s1 = new Square(12);
    s2 = s1->clone();
    s2->f();
    cout << s1->info() << endl;
    cout << s2->info() << endl;
}