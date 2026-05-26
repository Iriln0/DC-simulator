#pragma once
#include <string>

#include "./element.hpp"

class Diode : public Element{
public:
    Diode(std::string n, std::string n1, std::string n2):
                Element(std::move(n), {std::move(n1), std::move(n2)}) {}
    ~Diode() = default;

    ElementType getType() const override {
        return ElementType::Diode;
    }

    double getValue() const override {
        return _value;
    }

private:
    double _value;
};