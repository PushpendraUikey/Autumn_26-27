// 23B1023

#include <iostream>
#include <vector>
#include <string>

// 1. Component Interface
class IGraphic {
public:
    virtual ~IGraphic() = default;
    virtual void draw(int indent = 0) = 0;
    virtual void move() = 0;
    virtual IGraphic* copy() = 0;
    virtual bool operator==(const IGraphic& other) const = 0;
};


class Pen : public IGraphic {
    int ink = 0;
public:
    Pen(int ink = 0) : ink(ink) {}
    void draw(int indent = 0) override;
    void move() override;
    IGraphic* copy() override;
    virtual bool operator==(const IGraphic& other) const override;
};

class Brush : public IGraphic {
    int bristles = 0;
public:
    Brush(int bristles = 0) : bristles(bristles) {}
    void draw(int indent = 0) override;
    void move() override;
    IGraphic* copy() override;
    virtual bool operator==(const IGraphic& other) const override;
};

class Eraser : public IGraphic {
    int wear = 0;
public:
    Eraser(int wear = 0) : wear(wear) {}
    void draw(int indent = 0) override;
    void move() override;
    IGraphic* copy() override;
    virtual bool operator==(const IGraphic& other) const override;
};

class Shape : public IGraphic {
    int sides = 0;
public:
    Shape(int sides = 0) : sides(sides) {}
    void draw(int indent = 0) override;
    void move() override;
    IGraphic* copy() override;
    virtual bool operator==(const IGraphic& other) const override;
};

// 3. Composite Class
class Dialog : public IGraphic
{
public:
    Dialog() = default;
    ~Dialog();

    void addGraphic(IGraphic* graphic);
    void removeGraphic(IGraphic* graphic);
    
    void draw(int indent = 0) override;
    void move() override;
    IGraphic* copy() override;
    IGraphic* shallowCopy();
    virtual bool operator==(const IGraphic& other) const override;

private:
    std::vector<IGraphic*> _graphics; 
};