#include "inductor.h"

#include <utility>

Inductor::Inductor(std::string n, std::string n1, std::string n2, double v)
    : Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}

ElementType Inductor::getType() const {
    return ElementType::Inductor;
}

double Inductor::getValue() const {
    return _value;
}

const std::string& Inductor::getModel() const {
    static const std::string emptyModel = "";
    return emptyModel;
}