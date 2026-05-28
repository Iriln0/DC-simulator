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
     * @todo 部分选主元求解 A * x = b（A 为方阵的副本，不破坏原矩阵）。
     * MNA 中 A 为方阵；矩形最小二乘等需 QR，不在此接口范围。
     * b.size() 须等于 rows()；奇异时返回 false。
     */
    bool solve(std::vector<double>& x, const std::vector<double>& b) const;

    /**
     * @todo 对 m×n 矩阵做带部分选主元的 LUP 分解：PA = LU。
     * - P 为 m×m 行置换；L 为 m×min(m,n) 单位下三角（梯形）；U 为 min(m,n)×n 上三角（梯形）。
     * - 共 min(m-1, n) 步选主元与消元（n 阶方阵为 n-1 步）；分解结果覆写在 *this。
     * - _pivot 长度 m，初始 [0,1,...,m-1]，换行时同步交换。
     * 空矩阵或主元过小时返回 false。
     */
    bool luDecomposePartialPivot();

    /**
     * @todo 在 luDecomposePartialPivot() 成功后求解（视形状而定）：
     * - 方阵：A * x = b；
     * - 超定 m>n：最小二乘意义下的解（若实现）；
     * - 欠定 m<n：特解（若实现）。
     * 对 b 按 pivot 重排后前代、回代。
     */
    bool luSolvePartialPivot(std::vector<double>& x, const std::vector<double>& b) const;

    /** 是否已完成部分选主元 LU 分解（可供 luSolvePartialPivot 使用）。 */
    bool isFactorized() const;

    /** 行置换表 pivot，长度 rows()；初始 [0,1,...,m-1]，换行时同步交换。 */
    const std::vector<std::size_t>& pivotPermutation() const;

private:
    void invalidateFactorization();
    void initPivotPermutation();
    /** LUP 消元/选主元步数：min(rows()-1, cols())；n 阶方阵为 n-1 */
    std::size_t luStepCount() const;
    std::size_t findPivotRow(std::size_t step) const;
    void swapRows(std::size_t r1, std::size_t r2);
    static bool isPivotTooSmall(double pivot, double tolerance=1e-12);

    std::vector<std::vector<double>> _data;
    std::vector<std::size_t> _pivot;
    bool _factorized = false;
};
