#include "../include/matrix.h"

#include <cmath>
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
    at(row, col) += value;
}

const std::vector<std::vector<double>>& Matrix::data() const {
    return _data;
}

std::vector<std::vector<double>>& Matrix::data() {
    return _data;
}

void Matrix::multiply(Matrix& result, const Matrix& A, const Matrix& B) {
    if(A.empty() || B.empty()){
        throw std::invalid_argument("Matrix multiplication: empty matrix");
    }
    
    if(A.cols() != B.rows()){
        throw std::invalid_argument("Dimensions do not match");
    }

    if(&result == &A || &result == &B){
        Matrix tmp;
        multiply(tmp, A, B);
        result = std::move(tmp);
        return;
    }

    std::size_t r = A.rows();
    std::size_t c = B.cols();
    std::size_t t = A.cols();

    result.resize(r, c);
    for(size_t i = 0; i < r; ++i){
        for(size_t j = 0; j < c; ++j){
            double sum = 0.0;
            for(size_t k = 0; k < t; ++k){
                sum += A.at(i,k) * B.at(k,j);
            }
            result.at(i,j) = sum;
        }
    }
}

void Matrix::multiply(std::vector<double>& y, const std::vector<double>& x) const {
    if(this->empty() || x.empty()){
        throw std::invalid_argument("Matrix multiply vector: empty matrix or vector");
    }

    if(this->cols() != x.size()){
        throw std::invalid_argument("Dimensions do not match");
    }

    if(&y == &x){
        std::vector<double> tmp;
        multiply(tmp, x);
        y = std::move(tmp);
        return;
    }

    size_t col = this->rows();
    y.resize(col);
    for(size_t i = 0; i < col; ++i){
        double sum = 0.0;
        for(size_t j = 0; j < x.size(); ++j){
            sum += this->at(i,j) * x[j];
        }
        y[i] = sum;
    }
}

bool Matrix::solve(std::vector<double>& x, const std::vector<double>& b) const {
    (void)x;
    (void)b;

    /**
     * @todo
     * Partial-pivoting solve for A * x = b (A = *this).
     * Copy A, run luDecomposePartialPivot + luSolvePartialPivot on the copy,
     * or equivalent partial-pivot Gaussian elimination in one pass.
     */
    return false;
}

bool Matrix::luDecomposePartialPivot() {
    /**
     * @todo
     * PA = LU with partial row pivoting; store permutation in _pivot.
     * Set _factorized = true on success.
     */
    invalidateFactorization();
    return false;
}

bool Matrix::luSolvePartialPivot(std::vector<double>& x, const std::vector<double>& b) const {
    (void)x;
    (void)b;

    /**
     * @todo
     * Solve using PLU from luDecomposePartialPivot(); apply _pivot to b first.
     * Requires isFactorized() == true.
     */
    return false;
}

std::size_t Matrix::findPivotRow(std::size_t step){
    size_t idx = step;  // 主元的行索引
    double max_val = std::abs(this->at(step, step));    //主元值(in absolute value)
    for(std::size_t i = step+1; i < this->rows(); ++i){
        double cur_val = std::abs(this->at(i, step));
        if(cur_val > max_val){
            idx = i;
            max_val = cur_val;
        }
    }
    return idx;
}

void Matrix::swapRows(std::size_t r1, std::size_t r2){
    std::swap(_pivot[r1], _pivot[r2]);
    
    std::vector<double> tmp = _data[r1];
    _data[r1] = std::move(_data[r2]);
    _data[r2] = std::move(tmp);
}

bool Matrix::isPivotTooSmall(double pivot, double tolerance){
    if(std::abs(pivot) < tolerance){
        return true;
    }
    return false;
}
