#pragma once
#include <string>

#include "./element.h"

class VoltageSource : public Element{
public:
    VoltageSource(std::string n, std::string n1, std::string n2, double v);
    ~VoltageSource() override = default;

    ElementType getType() const override;

    double getValue() const override;

    const std::string& getModel() const override;

private:
    double _value;
};