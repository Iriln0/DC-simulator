#include "element.h"

#include <utility>

const char* elementTypeName(ElementType type){
    switch(type){
        case ElementType::Resistor: return "Resistor";
        case ElementType::Capacitor: return "Capacitor";
        case ElementType::Inductor: return "Inductor";
        case ElementType::VoltageSource: return "VoltageSource";
        case ElementType::CurrentSource: return "CurrentSource";
        case ElementType::Diode: return "Diode";
        case ElementType::BJT: return "BJT";
        case ElementType::MOSFET: return "MOSFET";
        default: return "Unknown";
    }
    return "Unknown";
}

Element::Element(std::string n, std::vector<std::string> nodes)
    : _name(std::move(n)), _nodes(std::move(nodes)) {}

Element::~Element() = default;

const std::string& Element::getName() const {
    return _name;
}

const std::vector<std::string>& Element::getNodes() const {
    return _nodes;
}

size_t Element::getTerminalsCount() const {
    return _nodes.size();
}