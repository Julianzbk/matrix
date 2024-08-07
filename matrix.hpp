#pragma once

#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>

using std::cout, std::endl;

class Matrix
{
public:
    const size_t num_rows; // height
    const size_t num_columns; // width
public: // TODO change to private after tests
    int* const buf; // class Matrix is sole owner
    class row
    {
    private:
        int* ptr;
        size_t width;
    public:
        row(int* ptr, size_t width)
            :ptr(ptr), width(width)
        {
        }

        int& operator [] (size_t column_index)
        {
            if ((column_index >= width) || (column_index < 0))
                throw std::domain_error("Index out of range!");
            return *(ptr + column_index);
        }
        const int& operator [] (size_t column_index) const;

        bool operator == (row const& other) const;
        bool operator == (std::vector<int> const& vec) const;

        // Potentially overflow
        row& operator++();
        row operator++(int);

        friend std::ostream& operator << (std::ostream& out, row R);
    };

    class column
    {
    private:
        int* ptr;
        size_t height;
        size_t num_columns;
    public:
        column(int* ptr, size_t height, size_t row_width)
            :ptr(ptr), height(height), num_columns(row_width)
        {
        }

        int& operator [] (size_t row_index)
        {
            if ((row_index >= height) || (row_index < 0))
                throw std::domain_error("Index out of range!");
            return *(ptr + num_columns * row_index);
        }
        const int& operator [] (size_t row_index) const;
        
        bool operator == (column const& other) const;
        bool operator == (std::vector<int> const& vec) const;
        
        column& operator++();
        column operator++(int);
         
        friend std::ostream& operator << (std::ostream& out, column C);
    };

    void fill(int value)
    {
        for (size_t m = 0; m < num_rows; ++m)
            for (size_t n = 0; n < num_columns; ++n)
                buf[m * num_columns + n] = value;
    }
    
    inline static int natural(size_t index)
    {
        return index >= 0 ? index - 1 : throw std::domain_error("Index out of range!");
    }
    
    inline bool in_range(size_t m, size_t n)
    {
        return (m < num_rows) && (n < num_columns) && (m >= 0) && (n >= 0);
    }

public:
    Matrix(size_t m, size_t n)
        :num_rows(m), num_columns(n), buf(new int[num_rows * num_columns])
    {
    }

    Matrix(std::vector<std::vector<int>> const& mtx)
        :num_rows(mtx.size()), num_columns(mtx[0].size()), buf(new int[num_rows * num_columns])
    {
        for (size_t i = 0; i < num_rows; ++i) {
            for (size_t j = 0; j < num_columns; ++j) {
                buf[i * num_columns + j] = mtx[i][j];
            }
        }
    }

    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other) = delete;
    Matrix(Matrix&& other);
    Matrix& operator=(Matrix&& other) = delete;

    friend std::ostream& operator << (std::ostream& out, Matrix M);

    row get_row(size_t row_index)
    {
        if ((row_index < 0) || (row_index >= num_rows))
            throw std::domain_error("Index out of range!");
        return row(buf + row_index * num_columns, num_columns);
    }
    row get_row(size_t row_index) const;

    column get_column(size_t column_index)
    {
        if ((column_index < 0) || (column_index >= num_columns))
            throw std::domain_error("Index out of range!");
        return column(buf + column_index, num_rows, num_columns);
    }
    column get_column(size_t column_index) const;

    row operator [] (size_t row_index)
    {
        return get_row(row_index);
    }
    row operator [] (size_t row_index) const;

    int& cell(size_t m, size_t n)
    {
        if (!in_range(m, n))
            throw std::domain_error("Indices out of range!");
        return buf[m * num_columns + n];
    }

    int& ncell(size_t m, size_t n)
    {
        m = natural(m);
        n = natural(n);
        if (!in_range(m, n))
            throw std::domain_error("Indices out of range!");
        return buf[m * num_columns + n];
    }

    bool operator == (Matrix const& other) const;
    bool operator == (std::vector<std::vector<int>> const& mtx) const;

    //Matrix operations

    inline static bool are_same_dimension(Matrix A, Matrix  B)
    {
        return (A.num_rows == B.num_rows) && (A.num_columns == B.num_columns);
    }

    inline static bool are_complementary(Matrix A, Matrix B)
    {
        return (A.num_rows == B.num_columns) && (A.num_columns == B.num_rows);
    }

    friend Matrix operator + (Matrix A, Matrix B);
    friend Matrix operator * (Matrix A, Matrix B);
    friend Matrix operator * (Matrix A, int scalar);
    friend Matrix operator * (int scalar, Matrix A)
    {
        return A * scalar;
    }

    ~Matrix()
    {
        delete[] buf;
    }
};