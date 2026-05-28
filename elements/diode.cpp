#include "diode.h"

#include <utility>

Diode::Diode(std::string n, std::string anode, std::string cathode, std::string model)
    : Element(std::move(n), {std::move(anode), std::move(cathode)}), _model(std::move(model)) {}

ElementType Diode::getType() const {
    return ElementType::Diode;
}

double Diode::getValue() const {
    return 0.0;
}

const std::string& Diode::getModel() const {
    return _model;
}

const std::string& Diode::anode() const {
    return _nodes[0];
}

const std::string& Diode::cathode() const {
    return _nodes[1];
}