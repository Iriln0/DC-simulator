#include <string>

#include "./element.hpp"

class Diode : public Element{
public:
    Diode(std::string n, std::string n1, std::string n2):
                Element(std::move(n), {std::move(n1), std::move(n2)}) {}
    ~Diode() = default;
    ElementType getType() const override {
        return ElementType::Diode;
    }

private:
    double _value;
};