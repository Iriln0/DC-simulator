#pragma once
#include <string>

class Resistor : public Element{
public:
    Resistor(std::string n, std::string n1, std::string n2, double v):
                _name(n), _nodes({n1, n2}), _value(v) {}
    ~Resistor() = default;

    ElementType getType() const override {
        return ElementType::Resistor;
    }

private:
    double _value;
};