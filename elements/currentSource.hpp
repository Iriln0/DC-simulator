#pragma once
#include <string>

#include "./element.hpp"

class CurrentSource : public Element{
public:
    CurrentSource(std::string n, std::string n1, std::string n2, double v):
                Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}
    ~CurrentSource() = default;

    ElementType getType() const override {
        return ElementType::CurrentSource;
    }

private:
    double _value;
};