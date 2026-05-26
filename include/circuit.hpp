#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "../elements/element.hpp"

class Circuit{
public:
    Circuit() = default;
    ~Circuit() = default;

    void addElement(std::unique_ptr<Element> element) {
        elements.push_back(std::move(element));
    }

private:
    std::vector<std::unique_ptr<Element>> elements;
};
