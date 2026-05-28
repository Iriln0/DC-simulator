#include "mosfet.h"

#include <utility>

MOSFET::MOSFET(std::string n, std::string drain, std::string gate, std::string source,
               std::string bulk, std::string model)
    : Element(std::move(n), {std::move(drain), std::move(gate), std::move(source),
                             std::move(bulk)}),
      _model(std::move(model)) {}

ElementType MOSFET::getType() const {
    return ElementType::MOSFET;
}

double MOSFET::getValue() const {
    return 0.0;
}

const std::string& MOSFET::getModel() const {
    return _model;
}

const std::string& MOSFET::drain() const {
    return _nodes[0];
}

const std::string& MOSFET::gate() const {
    return _nodes[1];
}

const std::string& MOSFET::source() const {
    return _nodes[2];
}

const std::string& MOSFET::bulk() const {
    return _nodes[3];
}