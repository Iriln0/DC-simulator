#include "../include/node_map.h"

#include <stdexcept>

int NodeMap::indexOf(const std::string& name) const{
    if(name == "0" || name == "GND"){
        return -1;
    }

    auto it = name_to_idx.find(name);
    if(it == name_to_idx.end()){
        throw std::runtime_error("Unknown node: " + name);
    }

    return it->second;
}

void NodeMap::buildFromCircuit(const Circuit& circuit){
    name_to_idx.clear();
    for(const auto& elem: circuit.getElements()){
        for(auto& n: elem->getNodes()){
            if(n != "0" && n != "GND" && !name_to_idx.count(n)){
                name_to_idx[n] = static_cast<int>(name_to_idx.size());
                idx_to_name.push_back(n);
            }
        }
    }
}

int NodeMap::nodeCount() const {
    return static_cast<int>(name_to_idx.size());
}

const std::vector<std::string>& NodeMap::nodeNameByIdx() const {
    return idx_to_name;
}