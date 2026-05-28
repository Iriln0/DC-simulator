#pragma once
#include <string>

#include "./element.h"

class MOSFET : public Element {
public:
    MOSFET(std::string n, std::string drain, std::string gate, std::string source,
           std::string bulk, std::string model);
    ~MOSFET() override = default;

    ElementType getType() const override;

    double getValue() const override;

    const std::string& getModel() const override;

    const std::string& drain() const;

    const std::string& gate() const;

    const std::string& source() const;

    const std::string& bulk() const;

private:
    std::string _model;
};
