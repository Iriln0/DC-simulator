#pragma once
#include <string>
#include <vector>

enum class ElementType{
    Resistor,
    Capacitor,
    Inductor,
    VoltageSource,
    CurrentSource,
    Diode,
    BJT,
    MOSFET
};

inline const char* elementTypeName(ElementType type){
    switch(type){
        case ElementType::Resistor:  return "Resistor";
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

class Element{
public:
    Element(std::string n, std::vector<std::string> nodes):
                _name(std::move(n)), _nodes(std::move(nodes)) {}
    virtual ~Element() = default;

    const std::string& getName() const {
        return _name;
    }

    const std::vector<std::string>& getNodes() const {
        return _nodes;
    }

    size_t getTerminalsCount() const {
        return _nodes.size();
    }

    virtual ElementType getType() const = 0;

    virtual double getValue() const = 0;

    virtual const std::string& getModel() const = 0;

protected:
    std::string _name;
    std::vector<std::string> _nodes;
};