#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "./circuit.h"
#include "./matrix.h"
#include "./node_map.h"

class DcSolver {
public:
    DcSolver() = default;
    ~DcSolver() = default;

    bool solve(Circuit& circuit, std::ostream& os = std::cout);

private:
    bool buildMnaSystem(const Circuit& circuit);
    int countVoltageSources(const Circuit& circuit) const;
    int countInductors(const Circuit& circuit) const;
    int nodeEqIndex(const std::string& node) const;
    int voltageSourceEqIndex(int vsOrdinal) const;
    int inductorEqIndex(int indOrdinal) const;

    bool assembleLinearSystem(const Circuit& circuit);

    bool stampResistor(const std::string& n1, const std::string& n2, double resistance);

    void stampCurrentSource(const std::string& nPlus, const std::string& nMinus, double current);

    void stampVoltageSource(const std::string& nPlus, const std::string& nMinus, double voltage,
                            int vsEqIndex);

    void stampInductorShort(const std::string& nPlus, const std::string& nMinus, int indEqIndex);

    void stampBranchVoltage(const std::string& nPlus, const std::string& nMinus, double voltage,
                            int eqRow);

    bool printOpResults(const Circuit& circuit, std::ostream& os) const;

    double nodeVoltage(const std::string& node) const;
    void printAllNodeVoltages(const char* sectionTitle, std::ostream& os) const;

    static bool isGroundNode(const std::string& node);
    static bool hasReferenceGround(Circuit& circuit);

    NodeMap nodeMap;
    Matrix jacobian;
    std::vector<double> rhs;
    std::vector<double> solution;

    int voltageSourceCount_ = 0;
    int inductorCount_ = 0;
    int systemSize_ = 0;
};
