#include "matrix.hpp"

#include <utility>


Matrix::Matrix(const Matrix& other)
    :num_rows(other.num_rows), num_columns(other.num_columns),
     buf(new int[num_rows * num_columns])
{
    for (size_t i = 0; i < num_rows * num_columns; ++i)
        buf[i] = other.buf[i];
}

Matrix::Matrix(Matrix&& other)
    :num_rows(other.num_rows), num_columns(other.num_columns),
     buf(std::move(other.buf))
{
}

// const vector/cell getters for const comparisons
Matrix::row Matrix::get_row(size_t row_index) const
{
    if ((row_index < 0) || (row_index >= num_rows))
        throw std::domain_error("Index out of range!");
    return Matrix::row(buf + row_index * num_columns, num_columns);
}

Matrix::column Matrix::get_column(size_t column_index) const
{
    if ((column_index < 0) || (column_index >= num_columns))
        throw std::domain_error("Index out of range!");
    return Matrix::column(buf + column_index, num_rows, num_columns);
}

Matrix::row Matrix::operator [] (size_t row_index) const
{
    return Matrix::get_row(row_index);
}

bool Matrix::operator == (Matrix const& other) const
{
    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_columns; ++j) {
            if ((*this)[i][j] != other[i][j])
                return false;
        }
    }
    return true;
}

bool Matrix::operator == (std::vector<std::vector<int>> const& mtx) const
{
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_columns; ++j) {
            if ((*this)[i][j] != mtx[i][j])
                return false;
        }
    }
    return true;
}

Matrix operator + (Matrix A, Matrix B)
{
    if (Matrix::are_same_dimension(A, B) == false)
        throw std::out_of_range("Matrices are not the same dimension");
    Matrix sum(A.num_rows, A.num_columns);
    for (size_t m = 0; m < A.num_rows; ++m)
        for (size_t n = 0; n < A.num_columns; ++n)
            sum[m][n] = A[m][n] + B[m][n];
    return sum;
}

Matrix operator * (Matrix A, Matrix B)
{
    if (Matrix::are_complementary(A, B) == false)
        throw std::out_of_range("Matrices are not complementary");
    Matrix product(A.num_rows, B.num_columns);
    for (size_t m = 0; m < A.num_rows; ++m) {
        for (size_t n = 0; n < B.num_columns; ++n) {
            Matrix::row vec1 = A.get_row(m);  // SLOW AF
            Matrix::column vec2 = B.get_column(n);
            int cell_product = 0;
            for (size_t k = 0; k < A.num_columns; ++k)
            {
                cell_product += vec1[k] * vec2[k];
            }
            product[m][n] = cell_product;
        }
    }
    return product;
}

Matrix operator * (Matrix A, int scalar)
{
    Matrix product(A);
    for (size_t m = 0; m < A.num_rows; ++m)
        for (size_t n = 0; n < A.num_columns; ++n)
            product[m][n] *= scalar;
    return product;
}

std::ostream& operator << (std::ostream& out, Matrix M)
{
    for (size_t i = 0; i < M.num_rows; ++i)
        out << "--";
    out << std::endl;
    for (size_t i = 0; i < M.num_rows; ++i) {
        for (size_t j = 0; j < M.num_columns; ++j) {
            out << M.buf[i * M.num_columns + j] << ' ';
        }
        out << '\n';
    }
    return out;
}

std::ostream& operator << (std::ostream& out, Matrix::row R)
{
    out << '[';
    for (size_t i = 0; i < R.width - 1; ++i)
    {
        out << R[i] << ' ';
    }
    out << R[R.width - 1];
    out << ']';
    return out;
}

std::ostream& operator << (std::ostream& out, Matrix::column C)
{
    out << "⎴\n";
    for (size_t i = 0; i < C.height; ++i)
    {
        out << C[i] << '\n';
    }
    out << "⎵";
    return out;
}


//Definitions for Matrix::row

const int& Matrix::row::operator [] (size_t column_index) const
{
    if ((column_index >= width) || (column_index < 0))
        throw std::domain_error("Index out of range!");
    return *(ptr + column_index);
}

bool Matrix::row::operator == (Matrix::row const& other) const
{
    for (size_t i = 0; i < width; ++i)
    {
        if (other[i] != (*this)[i])
            return false;
    }
    return true;
}

bool Matrix::row::operator == (std::vector<int> const& vec) const
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i] != (*this)[i])
            return false;
    }
    return true;
}

Matrix::row& Matrix::row::operator++()
{
    ptr += width;
    return *this;
}

Matrix::row Matrix::row::operator++(int)
{
    Matrix::row old = *this;
    Matrix::row::operator++();
    return old;
}


//Definitions for Matrix::column

const int& Matrix::column::operator [] (size_t row_index) const
{
    if ((row_index >= height) || (row_index < 0))
        throw std::domain_error("Index out of range!");
    return *(ptr + num_columns * row_index);
}

bool Matrix::column::operator == (Matrix::column const& other) const
{
    for (size_t i = 0; i < height; ++i)
    {
        if (other[i] != (*this)[i])
            return false;
    }
    return true;
}

bool Matrix::column::operator == (std::vector<int> const& vec) const
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i] != (*this)[i])
            return false;
    }
    return true;
}

Matrix::column& Matrix::column::operator++()
{
    ptr += num_columns;
    return *this;
}

Matrix::column Matrix::column::operator++(int)
{
    Matrix::column old = *this;
    Matrix::column::operator++();
    return old;
}
