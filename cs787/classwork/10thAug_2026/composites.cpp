// 23B1023

#include <iostream>
#include <algorithm>
#include <vector>
#include "composites.h"

void Pen::draw(int indent) {
    std::cout << std::string(indent, ' ') << "Drawing Pen" << std::endl;
}
void Pen::move() {
    std::cout << "Moving Pen" << std::endl;
}
IGraphic* Pen::copy() {
    std::cout << "Copying Pen" << std::endl;
    return new Pen(*this);
}
bool Pen::operator==(const IGraphic& other) const {
    const Pen* otherPen = dynamic_cast<const Pen*>(&other);
    return otherPen && (this->ink == otherPen->ink);
}

void Brush::draw(int indent) {
    std::cout << std::string(indent, ' ') << "Drawing Brush" << std::endl;
}
void Brush::move() {
    std::cout << "Moving Brush" << std::endl;
}
IGraphic* Brush::copy() {
    std::cout << "Copying Brush" << std::endl;
    return new Brush(*this);
}
bool Brush::operator==(const IGraphic& other) const {
    const Brush* otherBrush = dynamic_cast<const Brush*>(&other);
    return otherBrush && (this->bristles == otherBrush->bristles);
}

void Eraser::draw(int indent) {
    std::cout << std::string(indent, ' ') << "Drawing Eraser" << std::endl;
}
void Eraser::move() {
    std::cout << "Moving Eraser" << std::endl;
}
IGraphic* Eraser::copy() {
    std::cout << "Copying Eraser" << std::endl;
    return new Eraser(*this);
}
bool Eraser::operator==(const IGraphic& other) const {
    const Eraser* otherEraser = dynamic_cast<const Eraser*>(&other);
    return otherEraser && (this->wear == otherEraser->wear);
}

void Shape::draw(int indent) {
    std::cout << std::string(indent, ' ') << "Drawing Shape" << std::endl;
}
void Shape::move() {
    std::cout << "Moving Shape" << std::endl;
}
IGraphic* Shape::copy() {
    std::cout << "Copying Shape" << std::endl;
    return new Shape(*this);
}
bool Shape::operator==(const IGraphic& other) const {
    const Shape* otherShape = dynamic_cast<const Shape*>(&other);
    return otherShape && (this->sides == otherShape->sides);
}

/*
For this runtime destructor to work we must have a Virtual Destructor in the base class.
*/
Dialog::~Dialog() {
    for (IGraphic* graphic : _graphics) {
        delete graphic;
    }
}

void Dialog::draw(int indent) {
    std::cout << std::string(indent, ' ') << "Drawing Dialog" << std::endl;
    for (auto graphic : _graphics) {
        graphic->draw(indent + 4);
    }
}

void Dialog::move() {
    std::cout << "Moving Dialog" << std::endl;
    for (auto graphic : _graphics) {
        graphic->move();
    }
}

IGraphic* Dialog::copy() {
    std::cout << "Copying Dialog" << std::endl;
    Dialog* newDialog = new Dialog();
    for (auto graphic : _graphics) {
        newDialog->addGraphic(graphic->copy());
    }
    return newDialog;
}

void Dialog::addGraphic(IGraphic* graphic) {
    _graphics.push_back(graphic);
}

void Dialog::removeGraphic(IGraphic* graphic) {
    auto it = std::find(_graphics.begin(), _graphics.end(), graphic);
    if (it != _graphics.end()) {
        _graphics.erase(it);
        delete graphic;
    }
}

IGraphic* Dialog::shallowCopy() {
    std::cout << "Shallow Copying Dialog" << std::endl;
    Dialog* newDialog = new Dialog();
    for (auto graphic : _graphics) {
        newDialog->addGraphic(graphic);  // Shallow copy: just copying the pointers, not the objects themselves
    }
    return newDialog;
}

bool Dialog::operator==(const IGraphic& other) const {
    const Dialog* otherDialog = dynamic_cast<const Dialog*>(&other);
    if (!otherDialog) return false;
    if (_graphics.size() != otherDialog->_graphics.size()) return false;
    for (size_t i = 0; i < _graphics.size(); ++i) {
        if (!(*_graphics[i] == *otherDialog->_graphics[i])) {
            return false;
        }
    }
    return true;
}

int main() {
    Dialog* dialog1 = new Dialog();
    Dialog* dialog2 = new Dialog();
    Dialog* dialog3 = new Dialog();

    dialog2->addGraphic(new Pen(4));
    dialog2->addGraphic(new Brush(5));
    
    dialog1->addGraphic(dialog2);
    dialog1->addGraphic(new Eraser(6));

    dialog3->addGraphic(new Shape(7));
    dialog3->addGraphic(new Pen(8));

    dialog1->addGraphic(dialog3);

    dialog1->draw();
    dialog1->move();

    Dialog* dialogCopy = dynamic_cast<Dialog*>(dialog1->copy());

    if(*dialog1 == *dialogCopy) {
        std::cout << "dialog1 and dialogCopy are the same instance." << std::endl;
    } else {
        std::cout << "dialog1 and dialogCopy are different instances." << std::endl;
    }

    Dialog* dialogShallowCopy = dynamic_cast<Dialog*>(dialog1->shallowCopy());

    if(*dialog1 == *dialogShallowCopy) {
        std::cout << "dialog1 and dialogShallowCopy are the same instance." << std::endl;
    } else {
        std::cout << "dialog1 and dialogShallowCopy are different instances." << std::endl;
    }

    return 0;
}