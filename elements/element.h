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

const char* elementTypeName(ElementType type);

class Element{
public:
    Element(std::string n, std::vector<std::string> nodes);
    virtual ~Element();

    const std::string& getName() const;

    const std::vector<std::string>& getNodes() const;

    size_t getTerminalsCount() const;

    virtual ElementType getType() const = 0;

    virtual double getValue() const = 0;

    virtual const std::string& getModel() const = 0;

protected:
    std::string _name;
    std::vector<std::string> _nodes;
};