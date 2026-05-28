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
    // 任务 1：MNA 维数与节点/方程下标
    bool buildMnaSystem(const Circuit& circuit);
    int countVoltageSources(const Circuit& circuit) const;
    int nodeEqIndex(const std::string& node) const;
    int voltageSourceEqIndex(int vsOrdinal) const;

    // 任务 2：组装 G 与 rhs
    bool assembleLinearSystem(const Circuit& circuit);

    // 任务 3：线性元件 stamp
    bool stampResistor(const std::string& n1, const std::string& n2, double resistance);

    /**
     * @todo 独立电流源：电流从 n+ 流向 n-，只修改 rhs（KCL）。
     */
    void stampCurrentSource(const std::string& nPlus, const std::string& nMinus, double current);

    /**
     * @todo 独立电压源：MNA 扩维，节点行耦合 I_V，约束行 V(n+)-V(n-)=Vs。
     */
    void stampVoltageSource(const std::string& nPlus, const std::string& nMinus, double voltage,
                            int vsEqIndex);

    /**
     * @todo 根据 .OP / .PRINT 输出节点电压等。
     */
    bool printOpResults(const Circuit& circuit, std::ostream& os) const;

    NodeMap nodeMap;
    Matrix jacobian;
    std::vector<double> rhs;
    std::vector<double> solution;

    int voltageSourceCount_ = 0;
    int systemSize_ = 0;
};
