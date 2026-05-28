#include "capacitor.h"

#include <utility>

Capacitor::Capacitor(std::string n, std::string n1, std::string n2, double v)
    : Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}

ElementType Capacitor::getType() const {
    return ElementType::Capacitor;
}

double Capacitor::getValue() const {
    return _value;
}

const std::string& Capacitor::getModel() const {
    static const std::string emptyModel = "";
    return emptyModel;
}