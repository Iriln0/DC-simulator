#pragma once

#include <cstddef>
#include <iostream>
#include <vector>

// 基于 std::vector<std::vector<double>> 的稠密矩阵，供 MNA / Newton 迭代使用。
class Matrix {
public:
    Matrix() = default;
    Matrix(std::size_t rows, std::size_t cols, double fill = 0.0);

    std::size_t rows() const;
    std::size_t cols() const;
    bool empty() const;
    bool isSquare() const;

    void resize(std::size_t rows, std::size_t cols, double fill = 0.0);
    void clear();

    double& at(std::size_t row, std::size_t col);
    const double& at(std::size_t row, std::size_t col) const;

    void setZero();
    void setIdentity();

    /** MNA stamp：对 (row,col) 累加 value；会失效已有 LU 分解。 */
    void add(std::size_t row, std::size_t col, double value);

    const std::vector<std::vector<double>>& data() const;
    std::vector<std::vector<double>>& data();

    Matrix clone() const;
    void copyFrom(const Matrix& other);

    /** 方阵且 b 长度等于 rows() 时方可 solve / luSolve。 */
    bool canSolve(const std::vector<double>& b) const;

    /** 稠密矩阵乘法：result = A * B。 */
    static void multiply(Matrix& result, const Matrix& A, const Matrix& B);

    /** 矩阵-向量乘法：y = (*this) * x。 */
    void multiply(std::vector<double>& y, const std::vector<double>& x) const;

    /**
     * 部分选主元求解 A * x = b，在 *this 上原地分解（覆写矩阵为 LU）。
     * Newton 每轮重新组装后应优先使用本函数，避免整表拷贝。
     */
    bool solveInPlace(std::vector<double>& x, const std::vector<double>& b);

    /**
     * 求解 A * x = b：在副本上分解，不修改 *this 中的原始系数。
     */
    bool solve(std::vector<double>& x, const std::vector<double>& b) const;

    /** 对 m×n 矩阵做部分选主元 LUP：PA = LU，结果覆写在 *this。 */
    bool luDecomposePartialPivot();

    /** 在 luDecomposePartialPivot() 成功后求解 A * x = b（方阵）。 */
    bool luSolvePartialPivot(std::vector<double>& x, const std::vector<double>& b) const;

    bool isFactorized() const;
    const std::vector<std::size_t>& pivotPermutation() const;

    /** r = (*this) * x - b，用于 Newton 收敛判据。 */
    bool computeResidual(std::vector<double>& residual, const std::vector<double>& x,
                         const std::vector<double>& b) const;

    /** max_i |v[i]| */
    static double vectorNormInf(const std::vector<double>& v);

    /** 矩阵 ∞-范数：max_i sum_j |a_ij|。 */
    static double matrixNormInf(const Matrix& matrix);

    /** y += alpha * x */
    static void addScaled(std::vector<double>& y, const std::vector<double>& x, double alpha);

    /** x += dx（可选阻尼：x += damping * dx） */
    static void accumulate(std::vector<double>& x, const std::vector<double>& dx,
                           double damping = 1.0);

    void print(std::ostream& os = std::cout, int precision = 6) const;

    /**
     * 估计 cond_inf(A) ≈ ||A||_inf * ||A^{-1}||_inf（分别用行和与 n 次 LU 回代）。
     * 须在未分解状态下调用（setZero/stamp 后、luDecompose 前）。
     */
    bool estimateConditionNumber(double& condOut) const;

    /**
     * 原地 LU + 迭代 refinement：保存分解前系数，用 r = Ax-b 修正 x。
     * 须在未分解状态下调用；分解后 *this 为 LU 因子。
     */
    bool solveRefined(std::vector<double>& x, const std::vector<double>& b, int maxSteps = 1);

private:
    void invalidateFactorization();
    void initPivotPermutation();
    std::size_t luStepCount() const;
    std::size_t findPivotRow(std::size_t step) const;
    void swapRows(std::size_t r1, std::size_t r2);

    /** 第 step 步：在主元已就位后消去其下方（LUP 内层，由 luDecomposePartialPivot 调用）。 */
    bool eliminateBelowPivot(std::size_t step);

    /** 已分解时：||A^{-1}||_inf ≈ max_j ||A^{-1} e_j||_inf。 */
    double estimateInverseNormInf() const;

    static bool isPivotTooSmall(double pivot, double tolerance = 1e-12);

    std::vector<std::vector<double>> _data;
    std::vector<std::size_t> _pivot;
    bool _factorized = false;
};
