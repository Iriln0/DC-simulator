#pragma once
#include <stdexcept>
#include <string>
#include <vector>

#include "./element.h"

class BJT : public Element {
public:
    BJT(std::string n, std::string collector, std::string base, std::string emitter,
        std::string model, std::string substrate = "");
    ~BJT() override = default;

    ElementType getType() const override;

    double getValue() const override;

    const std::string& getModel() const override;

    const std::string& collector() const;

    const std::string& base() const;

    const std::string& emitter() const;

    bool hasSubstrate() const;

    const std::string& substrate() const;

private:
    std::string _model;
};
