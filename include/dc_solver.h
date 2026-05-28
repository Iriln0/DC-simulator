#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>

#include "./circuit.h"
#include "./node_map.h"
class DcSolver{
public:
    DcSolver() = default;
    ~DcSolver() = default;

    bool solve(Circuit& circuit, std::ostream& os=std::cout);

private:
    NodeMap nodeMap;
};