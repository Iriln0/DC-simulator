#pragma once
#include <string>
#include <utility>

#include "./element.hpp"

class Diode : public Element {
public:
    Diode(std::string n, std::string anode, std::string cathode, std::string model = "")
        : Element(std::move(n), {std::move(anode), std::move(cathode)}),
          _model(std::move(model)) {}
    ~Diode() = default;

    ElementType getType() const override {
        return ElementType::Diode;
    }

    double getValue() const override {
        return 0.0;
    }

    const std::string& getModel() const override {
        return _model;
    }

    const std::string& anode() const {
        return _nodes[0];
    }

    const std::string& cathode() const {
        return _nodes[1];
    }

private:
    std::string _model;
};
