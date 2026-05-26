#pragma once
#include <string>

class Capacitor : public Element{
public:
    Capacitor(std::string n, std::string n1, std::string n2, std::string v):
                Element(std::move(n), {std::move(n1), std::move(n2)}, v) {}
    ~Capacitor() = default;
        
    ElementType getType() const override {
        return ElementType::Capacitor;
    }

private:
    double _value;
};