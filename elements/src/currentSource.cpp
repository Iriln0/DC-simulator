#include "currentSource.h"

#include <utility>

CurrentSource::CurrentSource(std::string n, std::string n1, std::string n2, double v)
    : Element(std::move(n), {std::move(n1), std::move(n2)}), _value(v) {}

ElementType CurrentSource::getType() const {
    return ElementType::CurrentSource;
}

double CurrentSource::getValue() const {
    return _value;
}

const std::string& CurrentSource::getModel() const {
    static const std::string emptyModel = "";
    return emptyModel;
}