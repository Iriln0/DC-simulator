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

class Element{
public:
    Element(std::string n, std::vector<std::string> nodes, double v):
                _name(std::move(n)), _nodes(std::move(nodes)), _value(v) {}
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

protected:
    std::string _name;
    std::vector<std::string> _nodes;
};