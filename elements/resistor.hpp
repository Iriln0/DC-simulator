#pragma once
#include <string>

#include "./element.hpp"

class Resistor : public Element{
public:
    Resistor(std::string n, std::string n1, std::string n2, double v):
                Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}
    ~Resistor() = default;

    ElementType getType() const override {
        return ElementType::Resistor;
    }

private:
    double _value;
};