#pragma once

#include <cstddef>
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

    void add(std::size_t row, std::size_t col, double value);

    const std::vector<std::vector<double>>& data() const;
    std::vector<std::vector<double>>& data();

    /** 稠密矩阵乘法：result = A * B。 */
    static void multiply(Matrix& result, const Matrix& A, const Matrix& B);

    /** 矩阵-向量乘法：y = (*this) * x。 */
    void multiply(std::vector<double>& y, const std::vector<double>& x) const;

    /**
     * @todo 部分选主元求解 A * x = b（A 为当前方阵的副本，不破坏原矩阵）。
     * 可在内部调用 luDecomposePartialPivot + luSolvePartialPivot，或一次性消元。
     * b.size() 须等于 rows()；奇异时返回 false。
     */
    bool solve(std::vector<double>& x, const std::vector<double>& b) const;

    /**
     * @todo 对方阵做带部分选主元的 LU 分解：PA = LU。
     * 分解结果保存在 *this 的上三角 U、下三角 L（单位对角并入 L）中；
     * 行置换记录在 pivotPermutation()，即 pivot[k] 为第 k 步主元所在行。
     * 成功后 isFactorized() 为 true；奇异或非标方阵返回 false。
     */
    bool luDecomposePartialPivot();

    /**
     * @todo 在 luDecomposePartialPivot() 成功后，用已存储的 PLU 与行置换求解 A * x = b。
     * 实现时对 b 按 pivot 重排，再前代、回代。
     */
    bool luSolvePartialPivot(std::vector<double>& x, const std::vector<double>& b) const;

    /** 是否已完成部分选主元 LU 分解（可供 luSolvePartialPivot 使用）。 */
    bool isFactorized() const;

    /** 行置换表 pivot，长度 n；luDecomposePartialPivot 成功后有效。 */
    const std::vector<std::size_t>& pivotPermutation() const;

private:
    void invalidateFactorization();
    std::size_t findPivotRow(std::size_t step);
    void swapRows(std::size_t r1, std::size_t r2);
    static bool isPivotTooSmall(double pivot, double tolerance=1e-12);

    std::vector<std::vector<double>> _data;
    std::vector<std::size_t> _pivot;
    bool _factorized = false;
};
