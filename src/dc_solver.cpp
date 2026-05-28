#include "../include/dc_solver.h"

bool DcSolver::solve(Circuit& circuit, std::ostream& os) {
    (void)os;

    /**
     * @todo
     * Implement the DC solver:
     * 1. Build NodeMap and determine MNA size (nodes + voltage sources).
     * 2. Resize jacobian, rhs, solution; stamp linear/nonlinear elements each Newton step.
     * 3. Call jacobian.solve(solution, rhs) for J * dx = -F (see matrix.h @todo).
     * 4. Update solution until convergence; handle .OP / .PRINT output.
     */
    nodeMap.buildFromCircuit(circuit);
    (void)jacobian;
    (void)rhs;
    (void)solution;

    return true;
}