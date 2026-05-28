#pragma once
#include <string>
#include <unordered_map>

#include "./circuit.h"
// GND/"0" 不分配，返回 -1
class NodeMap{
    public:
    int indexOf(const std::string& name) const;

    void buildFromCircuit(const Circuit& circuit);

    int nodeCount() const;

    private:
    std::unordered_map<std::string,int> name_to_idx;
};