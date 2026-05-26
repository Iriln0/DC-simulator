#pragma once
#include <string>

#include "./element.hpp"

class Inductor : public Element{
public:
    Inductor(std::string n, std::string n1, std::string n2, double v):
                Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}
    ~Inductor() = default;

    ElementType getType() const override {
        return ElementType::Inductor;
    }

    double getValue() const override {
        return _value;
    }

private:
    double _value;
};