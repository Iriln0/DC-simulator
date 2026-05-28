#include "../include/dc_solver.h"

#include "../elements/bjt.h"
#include "../elements/currentSource.h"
#include "../elements/diode.h"
#include "../elements/inductor.h"
#include "../elements/mosfet.h"
#include "../elements/resistor.h"
#include "../elements/voltageSource.h"
#include "../include/device_eval.h"

int DcSolver::countVoltageSources(const Circuit& circuit) const {
    int count = 0;
    for (const auto& elem : circuit.getElements()) {
        if (elem->getType() == ElementType::VoltageSource) {
            ++count;
        }
    }
    return count;
}

int DcSolver::countInductors(const Circuit& circuit) const {
    int count = 0;
    for (const auto& elem : circuit.getElements()) {
        if (elem->getType() == ElementType::Inductor) {
            ++count;
        }
    }
    return count;
}

int DcSolver::nodeEqIndex(const std::string& node) const {
    return nodeMap.indexOf(node);
}

int DcSolver::voltageSourceEqIndex(int vsOrdinal) const {
    return nodeMap.nodeCount() + vsOrdinal;
}

int DcSolver::inductorEqIndex(int indOrdinal) const {
    return nodeMap.nodeCount() + voltageSourceCount_ + indOrdinal;
}

bool DcSolver::buildMnaSystem(const Circuit& circuit) {
    nodeMap.buildFromCircuit(circuit);
    voltageSourceCount_ = countVoltageSources(circuit);
    inductorCount_ = countInductors(circuit);
    systemSize_ = nodeMap.nodeCount() + voltageSourceCount_ + inductorCount_;

    if (systemSize_ == 0) {
        return false;
    }

    jacobian.resize(static_cast<std::size_t>(systemSize_),
                    static_cast<std::size_t>(systemSize_));
    rhs.assign(static_cast<std::size_t>(systemSize_), 0.0);
    solution.assign(static_cast<std::size_t>(systemSize_), 0.0);
    return true;
}

bool DcSolver::stampResistor(const std::string& n1, const std::string& n2, double resistance) {
    if (resistance == 0.0) {
        return false;
    }

    const double conductance = 1.0 / resistance;
    const int i = nodeEqIndex(n1);
    const int j = nodeEqIndex(n2);

    if (i >= 0) {
        jacobian.add(static_cast<std::size_t>(i), static_cast<std::size_t>(i), conductance);
    }
    if (j >= 0) {
        jacobian.add(static_cast<std::size_t>(j), static_cast<std::size_t>(j), conductance);
    }
    if (i >= 0 && j >= 0) {
        jacobian.add(static_cast<std::size_t>(i), static_cast<std::size_t>(j), -conductance);
        jacobian.add(static_cast<std::size_t>(j), static_cast<std::size_t>(i), -conductance);
    }

    return true;
}

void DcSolver::stampCurrentSource(const std::string& nPlus, const std::string& nMinus,
                                  double current) {
    int i = nodeEqIndex(nPlus);
    int j = nodeEqIndex(nMinus);

    if (i >= 0) {
        rhs[static_cast<std::size_t>(i)] -= current;
    }
    if (j >= 0) {
        rhs[static_cast<std::size_t>(j)] += current;
    }
}

void DcSolver::stampBranchVoltage(const std::string& nPlus, const std::string& nMinus,
                                  double voltage, int eqRow) {
    const int i = nodeEqIndex(nPlus);
    const int j = nodeEqIndex(nMinus);

    if (eqRow < 0) {
        return;
    }

    const std::size_t uk = static_cast<std::size_t>(eqRow);

    if (i >= 0) {
        jacobian.add(static_cast<std::size_t>(i), uk, 1.0);
        jacobian.add(uk, static_cast<std::size_t>(i), 1.0);
    }
    if (j >= 0) {
        jacobian.add(static_cast<std::size_t>(j), uk, -1.0);
        jacobian.add(uk, static_cast<std::size_t>(j), -1.0);
    }
    rhs[uk] = voltage;
}

void DcSolver::stampVoltageSource(const std::string& nPlus, const std::string& nMinus,
                                  double voltage, int vsEqIndex) {
    stampBranchVoltage(nPlus, nMinus, voltage, voltageSourceEqIndex(vsEqIndex));
}

void DcSolver::stampInductorShort(const std::string& nPlus, const std::string& nMinus,
                                  int indEqIndex) {
    stampBranchVoltage(nPlus, nMinus, 0.0, inductorEqIndex(indEqIndex));
}

bool DcSolver::stampLinearPart(const Circuit& circuit) {
    int vsOrdinal = 0;
    int indOrdinal = 0;
    for (const auto& elem : circuit.getElements()) {
        switch (elem->getType()) {
            case ElementType::Resistor: {
                const auto& resistor = static_cast<const Resistor&>(*elem);
                const auto& nodes = resistor.getNodes();
                if (!stampResistor(nodes[0], nodes[1], resistor.getValue())) {
                    return false;
                }
                break;
            }
            case ElementType::VoltageSource: {
                const auto& vs = static_cast<const VoltageSource&>(*elem);
                const auto& nodes = vs.getNodes();
                stampVoltageSource(nodes[0], nodes[1], vs.getValue(), vsOrdinal);
                ++vsOrdinal;
                break;
            }
            case ElementType::CurrentSource: {
                const auto& cs = static_cast<const CurrentSource&>(*elem);
                const auto& nodes = cs.getNodes();
                stampCurrentSource(nodes[0], nodes[1], cs.getValue());
                break;
            }
            case ElementType::Capacitor:
                break;
            case ElementType::Inductor: {
                const auto& inductor = static_cast<const Inductor&>(*elem);
                const auto& nodes = inductor.getNodes();
                stampInductorShort(nodes[0], nodes[1], indOrdinal);
                ++indOrdinal;
                break;
            }
            case ElementType::Diode:
            case ElementType::BJT:
            case ElementType::MOSFET:
                break;
            default:
                break;
        }
    }

    return true;
}

bool DcSolver::stampNonlinearPart(const Circuit& circuit) {
    (void)circuit;

    /**
     * @todo
     * 遍历 D/Q/M 元件，用 nodeVoltage() 取端口电压，调用 eval* 与 stampDiode/stampBjt/stampMosfet。
     * vsOrdinal / indOrdinal 仅由 stampLinearPart 维护，此处勿重复 stamp 线性支路。
     */
    return true;
}

bool DcSolver::assembleLinearSystem(const Circuit& circuit) {
    jacobian.setZero();
    std::fill(rhs.begin(), rhs.end(), 0.0);
    return stampLinearPart(circuit);
}

bool DcSolver::assembleSystem(const Circuit& circuit) {
    jacobian.setZero();
    std::fill(rhs.begin(), rhs.end(), 0.0);

    if (!stampLinearPart(circuit)) {
        return false;
    }

    return stampNonlinearPart(circuit);
}

void DcSolver::stampNorton(const std::string& nPlus, const std::string& nMinus, double g,
                           double ieq) {
    (void)nPlus;
    (void)nMinus;
    (void)g;
    (void)ieq;

    /**
     * @todo
     * 电导 g：与 stampResistor 相同拓扑（电导=g，电阻=1/g）；
     * 等效电流 ieq：stampCurrentSource(nPlus, nMinus, ieq)。
     */
}

void DcSolver::stampDiode(const Diode& diode, const Circuit& circuit) {
    (void)diode;
    (void)circuit;

    /**
     * @todo
     * Vd = nodeVoltage(anode) - nodeVoltage(cathode)；
     * params = circuit.models().get(diode.getModel()).diode()；
     * stampNorton(anode, cathode, evalDiode(Vd, params));
     */
}

void DcSolver::stampBjt(const BJT& bjt, const Circuit& circuit) {
    (void)bjt;
    (void)circuit;

    /**
     * @todo
     * 由 model 类型选 evalBjtNpn / evalBjtPnp，stamp C-B-E（及可选 substrate）伴随矩阵。
     */
}

void DcSolver::stampMosfet(const MOSFET& mosfet, const Circuit& circuit) {
    (void)mosfet;
    (void)circuit;

    /**
     * @todo
     * Vgs/Vds/Vbs 由四端节点电压得到，evalMosNmos / evalMosPmos，stamp D-G-S 伴随。
     */
}

bool DcSolver::circuitHasNonlinear(const Circuit& circuit) const {
    for (const auto& elem : circuit.getElements()) {
        switch (elem->getType()) {
            case ElementType::Diode:
            case ElementType::BJT:
            case ElementType::MOSFET:
                return true;
            default:
                break;
        }
    }
    return false;
}

bool DcSolver::obtainInitialGuess(const Circuit& circuit) {
    (void)circuit;

    /**
     * @todo
     * 1. assembleLinearSystem(circuit) 或 stampLinearPart + solve；
     * 2. 将解写入 solution 作为 Newton 初值；
     * 3. 可选：stampGminToGround 分档步进后再解。
     */
    return false;
}

bool DcSolver::runNewton(const Circuit& circuit) {
    (void)circuit;

    /**
     * @todo
     * for (iter < newtonMaxIter_):
     *   assembleSystem(circuit);
     *   jacobian.solveInPlace(solution, rhs);  // 原地 LU，无整表拷贝
     *   if (hasConverged(prev, solution)) return true;
     * 失败时 cerr 报告未收敛。
     */
    return false;
}

bool DcSolver::hasConverged(const std::vector<double>& prev,
                            const std::vector<double>& curr) const {
    (void)prev;
    (void)curr;

    /**
     * @todo
     * max_i |curr[i]-prev[i]| < newtonAbsTol_ + newtonRelTol_*max(|curr[i]|, 1)
     */
    return false;
}

void DcSolver::stampGminToGround(double gmin) {
    (void)gmin;

    /**
     * @todo
     * for i in 0 .. nodeMap.nodeCount()-1:
     *   jacobian.add(i, i, gmin)
     */
}

double DcSolver::nodeVoltage(const std::string& node) const {
    if (isGroundNode(node)) {
        return 0.0;
    }

    const int idx = nodeMap.indexOf(node);
    if (idx < 0 || static_cast<std::size_t>(idx) >= solution.size()) {
        return 0.0;
    }

    return solution[static_cast<std::size_t>(idx)];
}

void DcSolver::printAllNodeVoltages(const char* sectionTitle, std::ostream& os) const {
    os << sectionTitle << std::endl;
    for (const std::string& name : nodeMap.nodeNameByIdx()) {
        os << "V(" << name << ") = " << nodeVoltage(name) << std::endl;
    }
}

bool DcSolver::printOpResults(const Circuit& circuit, std::ostream& os) const {
    for (const Command& cmd : circuit.getCommands()) {
        switch (cmd.type) {
            case CommandType::Op:
                printAllNodeVoltages("---- DC Operating Point ----", os);
                break;
            case CommandType::Print:
                printAllNodeVoltages("---- DC Print ----", os);
                break;
            default:
                break;
        }
    }

    return true;
}

bool DcSolver::solve(Circuit& circuit, std::ostream& os) {
    if (!hasReferenceGround(circuit)) {
        std::cerr << "No reference node(0 or GND) in the Netlist" << std::endl;
        return false;
    }

    if (!buildMnaSystem(circuit)) {
        return false;
    }

    if (!circuitHasNonlinear(circuit)) {
        if (!assembleLinearSystem(circuit)) {
            return false;
        }
        if (!jacobian.solveInPlace(solution, rhs)) {
            return false;
        }
        return printOpResults(circuit, os);
    }

    /**
     * @todo
     * 非线性 DC：初值 + Newton。obtainInitialGuess / runNewton 实现后以下路径生效。
     */
    if (!obtainInitialGuess(circuit)) {
        std::cerr << "Failed to obtain initial guess for nonlinear DC analysis" << std::endl;
        return false;
    }

    if (!runNewton(circuit)) {
        std::cerr << "Newton iteration did not converge" << std::endl;
        return false;
    }

    return printOpResults(circuit, os);
}

bool DcSolver::isGroundNode(const std::string& node) {
    if (node == "0" || node == "GND") {
        return true;
    }

    return false;
}

bool DcSolver::hasReferenceGround(Circuit& circuit) {
    for (auto& element : circuit.getElements()) {
        for (auto& node : element->getNodes()) {
            if (isGroundNode(node)) {
                return true;
            }
        }
    }
    return false;
}
