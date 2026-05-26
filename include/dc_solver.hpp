#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>

#include "../include/circuit.hpp"
#include "../include/node_map.hpp"
class DcSolver{
public:
    DcSolver() = default;
    ~DcSolver() = default;

    bool solve(Circuit& circuit, std::ostream& os=std::cout) {
        /**
         * @todo
         * Implement the DC solver
         */
        nodeMap.buildFromCircuit(circuit);
    }

private:
    NodeMap nodeMap;
};