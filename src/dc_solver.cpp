#include "dc_solver.h"

bool DcSolver::solve(Circuit& circuit, std::ostream& os) {
    (void)os;

    /**
     * @todo
     * Implement the DC solver
     */
    nodeMap.buildFromCircuit(circuit);

    return true;
}