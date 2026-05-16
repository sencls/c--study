#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
float rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}
float sigmoidf(float x)
{
    return 1.f / (1.f + exp(-x));
}
class Matrix
{
public:
#define mat_printn(x) mat_print(x, #x)
    Matrix(size_t rows, size_t cols, float value = 0) : _rows(rows),
                                                        _cols(cols), matrix(rows, std::vector<float>(cols, value)) {}
    friend Matrix operator+(const Matrix &a, const Matrix &b)
    {
        assert(b._cols == a._cols && b._rows == a._rows);

        Matrix temp(a._rows, a._cols);
        for (int i = 0; i < a._rows; ++i)
        {
            for (int j = 0; j < a._cols; ++j)
            {
                temp.matrix[i][j] = a.matrix[i][j] + b.matrix[i][j];
            }
        }
        return temp;
    }

    friend Matrix operator*(const Matrix &a, const Matrix &b)
    {
        assert(a._cols == b._rows);
        Matrix t(a._rows, b._cols);
        int n = a._cols;
        for (int i = 0; i < a._rows; ++i)
        {
            for (int j = 0; j < b._cols; ++j)
            {
                for (int k = 0; k < n; k++)
                {
                    t.matrix[i][j] += a.matrix[i][k] * b.matrix[k][j];
                }
            }
        }
        return t;
    }
    friend void mat_print(Matrix &other, std::string name)
    {
        std::cout << name << "=[\n";
        for (const auto &t : other.matrix)
        {
            for (const auto &v : t)
            {
                std::cout << v << ' ';
            }
            std::cout << '\n';
        }
        std::cout << "]\n";
    }
    friend void mat_rand(Matrix &other, float low, float high)
    {

        for (auto &t : other.matrix)
        {
            for (auto &v : t)
            {
                v = rand_float() * (high - low) + low;
            }
        }
    }
    std::vector<float> &operator[](int x)
    {
        return matrix[x];
    }

    friend void mat_sig(Matrix &m)
    {
        for (int i = 0; i < m._rows; ++i)
        {
            for (int j = 0; j < m._cols; ++j)
            {
                m[j][j] = sigmoidf(m[i][j]);
            }
        }
    }

private:
    std::size_t _rows;
    std::size_t _cols;
    std::vector<std::vector<float>> matrix;
};
