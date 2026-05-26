#pragma once
#include <string>

class Inductor : public Element{
public:
    Inductor(std::string n, std::string n1, std::string n2, double v):
                _name(n), _nodes({n1, n2}), _value(v) {}
    ~Inductor() = default;

    ElementType getType() const override {
        return ElementType::Inductor;
    }

private:
    double _value;
};