#pragma once
#include <string>
#include "./element.h"

class Diode : public Element {
public:
    Diode(std::string n, std::string anode, std::string cathode, std::string model = "");
    ~Diode() override = default;

    ElementType getType() const override;

    double getValue() const override;

    const std::string& getModel() const override;

    const std::string& anode() const;

    const std::string& cathode() const;

private:
    std::string _model;
};
