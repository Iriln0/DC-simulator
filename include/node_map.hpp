#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

#include "./circuit.hpp"
// GND/"0" 不分配，返回 -1
class NodeMap{
    public:
    int indexOf(const std::string& name) const{
        if(name == "0" || name == "GND"){
            return -1;
        }
        
        auto it = name_to_idx.find(name);
        if(it == name_to_idx.end()){
            throw std::runtime_error("Unknown node: " + name);
        }
        
        return it->second;
    }

    void buildFromCircuit(const Circuit& circuit){
        name_to_idx.clear();
        for(const auto& elem: circuit.getElements()){
            for(auto& n: elem->getNodes()){
                if(n != "0" && n != "GND" && !name_to_idx.count(n)){
                    name_to_idx[n] = static_cast<int>(name_to_idx.size());
                }
            }
        }
    }

    int nodeCount() const { return static_cast<int>(name_to_idx.size()); }

    private:
    std::unordered_map<std::string,int> name_to_idx;
};