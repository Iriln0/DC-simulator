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

    /** MNA stamp：对 (row,col) 累加 value。 */
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
     * 部分选主元求解 A * x = b（在副本上分解，不修改 *this）。
     * 要求方阵且 b.size() == rows()。
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

    /** y += alpha * x */
    static void addScaled(std::vector<double>& y, const std::vector<double>& x, double alpha);

    /** x += dx（可选阻尼：x += damping * dx） */
    static void accumulate(std::vector<double>& x, const std::vector<double>& dx,
                           double damping = 1.0);

    void print(std::ostream& os = std::cout, int precision = 6) const;

    /**
     * @todo 估计矩阵条件数（如基于 LU 或 ∞-范数），用于病态检测。
     */
    bool estimateConditionNumber(double& condOut) const;

    /**
     * @todo 迭代 refinement：在已有 LU 分解基础上提高 Ax=b 的精度。
     */
    bool solveRefined(std::vector<double>& x, const std::vector<double>& b, int maxSteps = 1) const;

private:
    void invalidateFactorization();
    void initPivotPermutation();
    std::size_t luStepCount() const;
    std::size_t findPivotRow(std::size_t step) const;
    void swapRows(std::size_t r1, std::size_t r2);

    /** 第 step 步：在主元已就位后消去其下方（LUP 内层，由 luDecomposePartialPivot 调用）。 */
    bool eliminateBelowPivot(std::size_t step);

    static bool isPivotTooSmall(double pivot, double tolerance = 1e-12);

    std::vector<std::vector<double>> _data;
    std::vector<std::size_t> _pivot;
    bool _factorized = false;
};
