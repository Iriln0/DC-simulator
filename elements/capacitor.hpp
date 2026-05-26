#pragma once
#include <string>

#include "./element.hpp"

class Capacitor : public Element{
public:
    Capacitor(std::string n, std::string n1, std::string n2, double v):
                Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}
    ~Capacitor() = default;
        
    ElementType getType() const override {
        return ElementType::Capacitor;
    }

    double getValue() const override {
        return _value;
    }

private:
    double _value;
};