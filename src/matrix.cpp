#include "../include/matrix.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <stdexcept>

Matrix::Matrix(std::size_t rows, std::size_t cols, double fill) {
    resize(rows, cols, fill);
}

std::size_t Matrix::rows() const {
    return _data.size();
}

std::size_t Matrix::cols() const {
    if (_data.empty()) {
        return 0;
    }
    return _data[0].size();
}

bool Matrix::empty() const {
    return _data.empty() || cols() == 0;
}

bool Matrix::isSquare() const {
    return rows() == cols() && rows() > 0;
}

void Matrix::resize(std::size_t rowCount, std::size_t colCount, double fill) {
    _data.assign(rowCount, std::vector<double>(colCount, fill));
    invalidateFactorization();
}

void Matrix::clear() {
    _data.clear();
    invalidateFactorization();
}

void Matrix::invalidateFactorization() {
    _pivot.clear();
    _factorized = false;
}

bool Matrix::isFactorized() const {
    return _factorized;
}

const std::vector<std::size_t>& Matrix::pivotPermutation() const {
    return _pivot;
}

void Matrix::initPivotPermutation() {
    const std::size_t m = rows();
    _pivot.resize(m);
    for (std::size_t i = 0; i < m; ++i) {
        _pivot[i] = i;
    }
}

std::size_t Matrix::luStepCount() const {
    if (rows() == 0 || cols() == 0) {
        return 0;
    }
    return std::min(rows() - 1, cols());
}

Matrix Matrix::clone() const {
    return Matrix(*this);
}

void Matrix::copyFrom(const Matrix& other) {
    _data = other._data;
    _pivot = other._pivot;
    _factorized = other._factorized;
}

bool Matrix::canSolve(const std::vector<double>& b) const {
    return isSquare() && b.size() == rows();
}

double& Matrix::at(std::size_t row, std::size_t col) {
    if (row >= rows() || col >= cols()) {
        throw std::out_of_range("Matrix index out of range");
    }
    return _data[row][col];
}

const double& Matrix::at(std::size_t row, std::size_t col) const {
    if (row >= rows() || col >= cols()) {
        throw std::out_of_range("Matrix index out of range");
    }
    return _data[row][col];
}

void Matrix::setZero() {
    for (auto& row : _data) {
        for (double& value : row) {
            value = 0.0;
        }
    }
    invalidateFactorization();
}

void Matrix::setIdentity() {
    if (!isSquare()) {
        throw std::logic_error("setIdentity requires a square matrix");
    }
    setZero();
    for (std::size_t i = 0; i < rows(); ++i) {
        at(i, i) = 1.0;
    }
}

void Matrix::add(std::size_t row, std::size_t col, double value) {
    if (value == 0.0) {
        return;
    }
    at(row, col) += value;
}

const std::vector<std::vector<double>>& Matrix::data() const {
    return _data;
}

std::vector<std::vector<double>>& Matrix::data() {
    return _data;
}

void Matrix::multiply(Matrix& result, const Matrix& A, const Matrix& B) {
    if (A.empty() || B.empty()) {
        throw std::invalid_argument("Matrix multiplication: empty matrix");
    }

    if (A.cols() != B.rows()) {
        throw std::invalid_argument("Dimensions do not match");
    }

    if (&result == &A || &result == &B) {
        Matrix tmp;
        multiply(tmp, A, B);
        result = std::move(tmp);
        return;
    }

    const std::size_t r = A.rows();
    const std::size_t c = B.cols();
    const std::size_t t = A.cols();

    result.resize(r, c);
    for (std::size_t i = 0; i < r; ++i) {
        for (std::size_t j = 0; j < c; ++j) {
            double sum = 0.0;
            for (std::size_t k = 0; k < t; ++k) {
                sum += A.at(i, k) * B.at(k, j);
            }
            result.at(i, j) = sum;
        }
    }
}

void Matrix::multiply(std::vector<double>& y, const std::vector<double>& x) const {
    if (empty() || x.empty()) {
        throw std::invalid_argument("Matrix multiply vector: empty matrix or vector");
    }

    if (cols() != x.size()) {
        throw std::invalid_argument("Dimensions do not match");
    }

    if (&y == &x) {
        std::vector<double> tmp;
        multiply(tmp, x);
        y = std::move(tmp);
        return;
    }

    const std::size_t m = rows();
    y.resize(m);
    for (std::size_t i = 0; i < m; ++i) {
        double sum = 0.0;
        for (std::size_t j = 0; j < x.size(); ++j) {
            sum += at(i, j) * x[j];
        }
        y[i] = sum;
    }
}

bool Matrix::computeResidual(std::vector<double>& residual, const std::vector<double>& x,
                             const std::vector<double>& b) const {
    if (!canSolve(b) || x.size() != b.size()) {
        return false;
    }

    multiply(residual, x);
    if (residual.size() != b.size()) {
        return false;
    }

    for (std::size_t i = 0; i < b.size(); ++i) {
        residual[i] -= b[i];
    }
    return true;
}

double Matrix::vectorNormInf(const std::vector<double>& v) {
    double maxAbs = 0.0;
    for (double value : v) {
        maxAbs = std::max(maxAbs, std::abs(value));
    }
    return maxAbs;
}

void Matrix::addScaled(std::vector<double>& y, const std::vector<double>& x, double alpha) {
    if (y.size() != x.size()) {
        throw std::invalid_argument("addScaled: vector size mismatch");
    }
    for (std::size_t i = 0; i < y.size(); ++i) {
        y[i] += alpha * x[i];
    }
}

void Matrix::accumulate(std::vector<double>& x, const std::vector<double>& dx, double damping) {
    if (x.size() != dx.size()) {
        throw std::invalid_argument("accumulate: vector size mismatch");
    }
    for (std::size_t i = 0; i < x.size(); ++i) {
        x[i] += damping * dx[i];
    }
}

void Matrix::print(std::ostream& os, int precision) const {
    os << "Matrix " << rows() << " x " << cols();
    if (_factorized) {
        os << " (factorized)";
    }
    os << "\n";
    os << std::scientific << std::setprecision(precision);
    for (std::size_t i = 0; i < rows(); ++i) {
        for (std::size_t j = 0; j < cols(); ++j) {
            os << at(i, j) << " ";
        }
        os << "\n";
    }
    os << std::defaultfloat;
}

bool Matrix::solveInPlace(std::vector<double>& x, const std::vector<double>& b) {
    if (!canSolve(b)) {
        return false;
    }

    if (!luDecomposePartialPivot()) {
        return false;
    }

    return luSolvePartialPivot(x, b);
}

bool Matrix::solve(std::vector<double>& x, const std::vector<double>& b) const {
    if (!canSolve(b)) {
        return false;
    }

    Matrix temp(*this);
    return temp.solveInPlace(x, b);
}

bool Matrix::luDecomposePartialPivot() {
    invalidateFactorization();
    if (empty()) {
        return false;
    }

    initPivotPermutation();

    const std::size_t steps = luStepCount();
    for (std::size_t step = 0; step < steps; ++step) {
        const std::size_t p_row = findPivotRow(step);
        if (isPivotTooSmall(std::abs(at(p_row, step)))) {
            invalidateFactorization();
            return false;
        }
        if (p_row != step) {
            swapRows(step, p_row);
        }

        if (!eliminateBelowPivot(step)) {
            invalidateFactorization();
            return false;
        }
    }

    _factorized = true;
    return true;
}

bool Matrix::eliminateBelowPivot(std::size_t step) {
    const double pivot = at(step, step);
    if (isPivotTooSmall(std::abs(pivot))) {
        return false;
    }

    for (std::size_t i = step + 1; i < rows(); ++i) {
        const double lambda = at(i, step) / pivot;
        for (std::size_t j = step + 1; j < cols(); ++j) {
            at(i, j) -= lambda * at(step, j);
        }
        at(i, step) = lambda;
    }
    return true;
}

bool Matrix::luSolvePartialPivot(std::vector<double>& x, const std::vector<double>& b) const {
    if (!isFactorized() || !canSolve(b)) {
        return false;
    }

    if (_pivot.size() != rows()) {
        return false;
    }

    x.resize(b.size());
    for (std::size_t i = 0; i < b.size(); ++i) {
        x[i] = b[_pivot[i]];
    }

    // forward: Ly = Pb
    for (std::size_t i = 0; i < x.size(); ++i) {
        for (std::size_t j = 0; j < i; ++j) {
            x[i] -= x[j] * at(i, j);
        }
    }

    // backward: Ux = y
    if (x.empty()) {
        return false;
    }

    for (int i = static_cast<int>(x.size()) - 1; i >= 0; --i) {
        for (int j = static_cast<int>(x.size()) - 1; j > i; --j) {
            x[static_cast<std::size_t>(i)] -= x[static_cast<std::size_t>(j)] *
                                               at(static_cast<std::size_t>(i),
                                                  static_cast<std::size_t>(j));
        }
        const double diag = at(static_cast<std::size_t>(i), static_cast<std::size_t>(i));
        if (isPivotTooSmall(std::abs(diag))) {
            return false;
        }
        x[static_cast<std::size_t>(i)] /= diag;
    }

    return true;
}

std::size_t Matrix::findPivotRow(std::size_t step) const {
    std::size_t idx = step;
    double maxVal = std::abs(at(step, step));
    for (std::size_t i = step + 1; i < rows(); ++i) {
        const double curVal = std::abs(at(i, step));
        if (curVal > maxVal) {
            idx = i;
            maxVal = curVal;
        }
    }
    return idx;
}

void Matrix::swapRows(std::size_t r1, std::size_t r2) {
    if (r1 == r2) {
        return;
    }
    std::swap(_pivot[r1], _pivot[r2]);
    std::swap(_data[r1], _data[r2]);
}

bool Matrix::isPivotTooSmall(double pivot, double tolerance) {
    return std::abs(pivot) < tolerance;
}

double Matrix::matrixNormInf(const Matrix& matrix) {
    if (matrix.empty()) {
        return 0.0;
    }

    double normA = 0.0;
    for (std::size_t i = 0; i < matrix.rows(); ++i) {
        double rowSum = 0.0;
        for (std::size_t j = 0; j < matrix.cols(); ++j) {
            rowSum += std::abs(matrix.at(i, j));
        }
        normA = std::max(normA, rowSum);
    }
    return normA;
}

double Matrix::estimateInverseNormInf() const {
    if (!isFactorized() || !isSquare() || empty()) {
        return 0.0;
    }

    const std::size_t n = rows();
    std::vector<double> e(n, 0.0);
    std::vector<double> col(n, 0.0);
    double normInv = 0.0;

    for (std::size_t j = 0; j < n; ++j) {
        std::fill(e.begin(), e.end(), 0.0);
        e[j] = 1.0;
        if (!luSolvePartialPivot(col, e)) {
            return 0.0;
        }
        normInv = std::max(normInv, vectorNormInf(col));
    }

    return normInv;
}

bool Matrix::estimateConditionNumber(double& condOut) const {
    if (!isSquare() || empty()) {
        return false;
    }

    if (_factorized) {
        return false;
    }

    const double normA = matrixNormInf(*this);
    if (normA == 0.0) {
        condOut = 0.0;
        return true;
    }

    Matrix lu(*this);
    if (!lu.luDecomposePartialPivot()) {
        return false;
    }

    const double normInvA = lu.estimateInverseNormInf();
    if (normInvA == 0.0) {
        return false;
    }

    condOut = normA * normInvA;
    return std::isfinite(condOut);
}

bool Matrix::solveRefined(std::vector<double>& x, const std::vector<double>& b, int maxSteps) {
    if (!canSolve(b) || maxSteps < 1) {
        return false;
    }

    if (_factorized) {
        return false;
    }

    const Matrix original(*this);
    if (!luDecomposePartialPivot()) {
        return false;
    }

    if (!luSolvePartialPivot(x, b)) {
        return false;
    }

    std::vector<double> residual;
    std::vector<double> correction;

    for (int step = 0; step < maxSteps; ++step) {
        if (!original.computeResidual(residual, x, b)) {
            return false;
        }

        const double residualNorm = vectorNormInf(residual);
        const double scale = std::max(vectorNormInf(b), 1.0);
        if (residualNorm <= 1e-15 * scale) {
            break;
        }

        if (!luSolvePartialPivot(correction, residual)) {
            return false;
        }

        for (std::size_t i = 0; i < x.size(); ++i) {
            x[i] -= correction[i];
        }
    }

    return true;
}
