#include "voltageSource.h"

#include <utility>

VoltageSource::VoltageSource(std::string n, std::string n1, std::string n2, double v)
    : Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}

ElementType VoltageSource::getType() const {
    return ElementType::VoltageSource;
}

double VoltageSource::getValue() const {
    return _value;
}

const std::string& VoltageSource::getModel() const {
    static const std::string emptyModel = "";
    return emptyModel;
}