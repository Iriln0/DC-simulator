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

    /** 仅线性元件（R/V/I/C 开路/L 短路），用于初值与纯线性网表。 */
    bool assembleLinearSystem(const Circuit& circuit);

    /** 线性 + 非线性伴随，Newton 每轮调用。 */
    bool assembleSystem(const Circuit& circuit);

    bool stampLinearPart(const Circuit& circuit);
    bool stampNonlinearPart(const Circuit& circuit);

    void stampNorton(const std::string& nPlus, const std::string& nMinus, double g, double ieq);

    void stampDiode(const class Diode& diode, const Circuit& circuit);
    void stampBjt(const class BJT& bjt, const Circuit& circuit);
    void stampMosfet(const class MOSFET& mosfet, const Circuit& circuit);

    bool circuitHasNonlinear(const Circuit& circuit) const;
    bool obtainInitialGuess(const Circuit& circuit);
    bool runNewton(const Circuit& circuit);
    bool hasConverged(const std::vector<double>& prev,
                      const std::vector<double>& curr) const;

    /**
     * @todo
     * 各节点对地并联 gmin（收敛辅助）；在 obtainInitialGuess 或 runNewton 前调用。
     */
    void stampGminToGround(double gmin);

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

    /** Newton 迭代参数（可在 obtainInitialGuess / runNewton 中使用）。 */
    int newtonMaxIter_ = 50;
    double newtonRelTol_ = 1e-6;
    double newtonAbsTol_ = 1e-9;
    double newtonDamping_ = 1.0;
};
