#pragma once
#include <string>
#include <utility>
#include <vector>

#include "./element.hpp"

class MOSFET : public Element {
public:
    MOSFET(std::string n, std::string drain, std::string gate, std::string source,
           std::string bulk, std::string model)
        : Element(std::move(n), {std::move(drain), std::move(gate), std::move(source),
                                 std::move(bulk)}),
          _model(std::move(model)) {}
    ~MOSFET() = default;

    ElementType getType() const override {
        return ElementType::MOSFET;
    }

    double getValue() const override {
        return 0.0;
    }

    const std::string& getModel() const {
        return _model;
    }

    const std::string& drain() const {
        return _nodes[0];
    }

    const std::string& gate() const {
        return _nodes[1];
    }

    const std::string& source() const {
        return _nodes[2];
    }

    const std::string& bulk() const {
        return _nodes[3];
    }

private:
    std::string _model;
};
