#pragma once
#include <string>

class Capacitor{
public:
    Capacitor(std::string n, std::string n1, std::string n2, std::string v):
                name(n), node1(n1), node2(n2), value(v) {}
    ~Capacitor() = default;

private:
    std::string name;
    std::string node1;
    std::string node2;
    std::string value;
};