#pragma once
#include <string>

#include "./element.h"

class Inductor : public Element{
public:
    Inductor(std::string n, std::string n1, std::string n2, double v);
    ~Inductor() override = default;

    ElementType getType() const override;

    double getValue() const override;

    const std::string& getModel() const override;

private:
    double _value;
};