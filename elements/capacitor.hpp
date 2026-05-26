#pragma once
#include <string>

class Capacitor : public Element{
public:
    Capacitor(std::string n, std::string n1, std::string n2, std::string v):
                name(n), node1(n1), node2(n2), value(v) {}
    ~Capacitor() = default;

private:
    double _value;
};