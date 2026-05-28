#include "resistor.h"

#include <utility>

Resistor::Resistor(std::string n, std::string n1, std::string n2, double v)
    : Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}

ElementType Resistor::getType() const {
    return ElementType::Resistor;
}

double Resistor::getValue() const {
    return _value;
}

const std::string& Resistor::getModel() const {
    static const std::string emptyModel = "";
    return emptyModel;
}