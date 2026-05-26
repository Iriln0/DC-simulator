#pragma once
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "./element.hpp"

class BJT : public Element {
public:
    BJT(std::string n, std::string collector, std::string base, std::string emitter,
        std::string model, std::string substrate = "")
        : Element(std::move(n), makeNodes(collector, base, emitter, substrate)),
          _model(std::move(model)) {}
    ~BJT() = default;

    ElementType getType() const override {
        return ElementType::BJT;
    }

    double getValue() const override {
        return 0.0;
    }

    const std::string& getModel() const {
        return _model;
    }

    const std::string& collector() const {
        return _nodes[0];
    }

    const std::string& base() const {
        return _nodes[1];
    }

    const std::string& emitter() const {
        return _nodes[2];
    }

    bool hasSubstrate() const {
        return _nodes.size() == 4;
    }

    const std::string& substrate() const {
        if (!hasSubstrate()) {
            throw std::logic_error(
                "BJT::substrate(): device has no substrate terminal (C-B-E only)");
        }
        return _nodes[3];
    }

private:
    static std::vector<std::string> makeNodes(const std::string& collector,
                                                const std::string& base,
                                                const std::string& emitter,
                                                const std::string& substrate) {
        if (substrate.empty()) {
            return {collector, base, emitter};
        }
        return {collector, base, emitter, substrate};
    }

    std::string _model;
};
