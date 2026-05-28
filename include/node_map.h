#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "./circuit.h"
// GND/"0" 不分配，返回 -1
class NodeMap{
    public:
    int indexOf(const std::string& name) const;

    void buildFromCircuit(const Circuit& circuit);

    int nodeCount() const;

    const std::vector<std::string>& nodeNameByIdx() const;

    private:
    std::unordered_map<std::string,int> name_to_idx;
    std::vector<std::string> idx_to_name;
};