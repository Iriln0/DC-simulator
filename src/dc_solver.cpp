#include "../include/dc_solver.h"

#include "../elements/currentSource.h"
#include "../elements/resistor.h"
#include "../elements/voltageSource.h"

int DcSolver::countVoltageSources(const Circuit& circuit) const {
    int count = 0;
    for (const auto& elem : circuit.getElements()) {
        if (elem->getType() == ElementType::VoltageSource) {
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

bool DcSolver::buildMnaSystem(const Circuit& circuit) {
    nodeMap.buildFromCircuit(circuit);
    voltageSourceCount_ = countVoltageSources(circuit);
    systemSize_ = nodeMap.nodeCount() + voltageSourceCount_;

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
    (void)nPlus;
    (void)nMinus;
    (void)current;

    /**
     * @todo
     * Independent current source: +I into n-, -I from n+ in KCL rhs.
     */
    int i = nodeEqIndex(nPlus);
    int j = nodeEqIndex(nMinus);

    if(i >=0){
        rhs[static_cast<std::size_t>(i)] -= current;
    }
    if(j >=0){
        rhs[static_cast<std::size_t>(j)] += current;
    }
}

void DcSolver::stampVoltageSource(const std::string& nPlus, const std::string& nMinus,
                                  double voltage, int vsEqIndex) {
    (void)nPlus;
    (void)nMinus;
    (void)voltage;
    (void)vsEqIndex;

    /**
     * @todo
     * Stamp MNA rows for V(n+) - V(n-) = voltage using vsEqIndex column/row.
     */
    int i = nodeEqIndex(nPlus);
    int j = nodeEqIndex(nMinus);
    int k = voltageSourceEqIndex(vsEqIndex);

    if(k < 0){
        return;
    }

    const std::size_t uk = static_cast<std::size_t>(k);

    if(i >= 0){
        jacobian.add(static_cast<std::size_t>(i), uk, 1.0);
        jacobian.add(uk, static_cast<std::size_t>(i), 1.0);
    }
    if(j >= 0){
        jacobian.add(static_cast<std::size_t>(j), uk, -1.0);
        jacobian.add(uk, static_cast<std::size_t>(j), -1.0);
    }
    rhs[uk] = voltage;
}

bool DcSolver::assembleLinearSystem(const Circuit& circuit) {
    jacobian.setZero();
    std::fill(rhs.begin(), rhs.end(), 0.0);

    int vsOrdinal = 0;
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
            case ElementType::Inductor:
                // DC: C 开路、L 短路；线性里程碑先不 stamp
                break;
            case ElementType::Diode:
            case ElementType::BJT:
            case ElementType::MOSFET:
                /**
                 * @todo
                 * Nonlinear devices: stamp equivalent conductance + current in Newton loop.
                 */
                break;
            default:
                break;
        }
    }

    return true;
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
    if(!hasReferenceGround(circuit)){
        std::cerr << "No reference node(0 or GND) in the Netlist" << std::endl;
        return false;
    }

    if (!buildMnaSystem(circuit)) {
        return false;
    }

    if (!assembleLinearSystem(circuit)) {
        return false;
    }

    /**
     * @todo
     * Linear DC: one-shot solve. Nonlinear: Newton loop with re-assemble each iteration.
     */
    if (!jacobian.solve(solution, rhs)) {
        return false;
    }

    return printOpResults(circuit, os);
}

bool DcSolver::isGroundNode(const std::string& node){
    if(node == "0" || node == "GND"){
        return true;
    }

    return false;
}

bool DcSolver::hasReferenceGround(Circuit& circuit){
    for(auto& element: circuit.getElements()){
        for(auto& node: element->getNodes()){
            if(isGroundNode(node)){
                return true;
            }
        }
    }
    return false;
}