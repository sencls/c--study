#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <string>
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
    Matrix() {}
    Matrix(size_t rows, size_t cols, std::vector<std::vector<float>> t) : _rows(rows), _cols(cols), matrix(t) {}
    Matrix(size_t rows, size_t cols, float value = 0.f) : _rows(rows), _cols(cols), matrix(std::vector<std::vector<float>>(rows, std::vector<float>(cols, value))) {}
    friend void mat_size(Matrix &m, size_t rows, size_t cols)
    {
        m._cols = cols;
        m._rows = rows;
        std::vector<std::vector<float>> t(rows, std::vector<float>(cols, 0.f));
        m.matrix = std::move(t);
    }

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

#define mat_printnl(x, padding, i) mat_print(x, #x, padding, i)
    friend void mat_print(Matrix other, std::string name, size_t padding, int cnt)
    {
        name.erase(name.begin() + name.find("m."));
        name.erase(name.begin());
        name[name.find("[") + 1] = '0' + cnt;
        for (int i = 0; i < padding; ++i)
            std::cout << " ";
        std::cout << name << "=[\n";
        for (const auto &t : other.matrix)
        {
            std::cout << "     ";
            for (const auto &v : t)
            {
                std::cout << v << ' ';
            }
            std::cout << '\n';
        }
        for (int i = 0; i < padding; ++i)
            std::cout << " ";
        std::cout << "]\n";
    }
#define mat_printn(x) mat_print(x, #x)
    friend void mat_print(Matrix other, std::string name)
    {

        std::cout << " ";
        std::cout << name << "=[\n";
        for (const auto &t : other.matrix)
        {
            std::cout << "     ";
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

                m[i][j] = sigmoidf(m[i][j]);
            }
        }
    }

    friend Matrix mat_row(Matrix m, size_t row)
    {
        Matrix t(1, m._cols);
        t[0] = m[row];
        return t;
    }
    friend void mat_copy(Matrix &dst, Matrix src)
    {
        assert(dst._rows == src._rows);
        assert(dst._cols == src._cols);
        for (int i = 0; i < dst._rows; ++i)
        {
            for (int j = 0; j < src._cols; ++j)
            {
                dst[i][j] = src[i][j];
            }
        }
    }
    const size_t getcol() const
    {
        return _cols;
    }
    const size_t getrow() const
    {
        return _rows;
    }
    friend Matrix mat_sub(Matrix m, size_t x, size_t y, size_t xt = 0, size_t yt = 0)
    {
        Matrix t(x, y);
        int row = x + xt, col = y + yt;
        for (int i = xt; i < row; ++i)
        {
            for (int j = yt; j < col; ++j)
            {
                t[i - xt][j - yt] = m[i][j];
            }
        }
        return t;
    }

private:
    std::size_t _rows = 0;
    std::size_t _cols = 0;
    std::vector<std::vector<float>> matrix;
};
struct Xor
{
    size_t count;
    std::vector<Matrix> ws, bs, as;
};
Xor create(std::vector<size_t> arch, size_t arch_count)
{
    Xor m;
    m.count = arch_count - 1;
    m.bs.resize(m.count);
    m.ws.resize(m.count);
    m.as.resize(m.count + 1);
    mat_size(m.as[0], 1, arch[0]);
    for (int i = 1; i < arch_count; ++i)
    {
        mat_size(m.ws[i - 1], m.as[i - 1].getcol(), arch[i]);
        mat_size(m.bs[i - 1], 1, arch[i]);
        mat_size(m.as[i], 1, arch[i]);
    }
    return m;
}
#define print_xorn(x) print_xor(x, #x)
void Xor_random(Xor &m, float low, float high)
{
    for (int i = 0; i < m.count; ++i)
    {
        mat_rand(m.ws[i], low, high);
        mat_rand(m.bs[i], low, high);
    }
}
void print_xor(const Xor &m, std::string name)
{
    std::cout << name << "=[\n";
    for (size_t i = 0; i < m.count; ++i)
    {
        mat_printnl(m.ws[i], 4, i);
        mat_printnl(m.bs[i], 4, i);
    }
    std::cout << "]\n";
}
void Xor_forward(Xor &m)
{
    for (int i = 0; i < m.count; ++i)
    {

        m.as[i + 1] = (m.as[i]) * (m.ws[i]) + m.bs[i];
        mat_sig(m.as[i + 1]);
    }
}
#define INPUT(x) (x).as[0]
#define OUTPUT(x) (x).as[(x).count]
float Xor_cost(Xor m, Matrix &ti, Matrix &to)
{
    assert(ti.getrow() == to.getrow());
    assert(to.getcol() == OUTPUT(m).getcol());
    size_t n = ti.getrow();
    float c = 0;
    for (int i = 0; i < n; ++i)
    {
        Matrix x = mat_row(ti, i);
        Matrix y = mat_row(to, i);
        mat_copy(INPUT(m), x);
        Xor_forward(m);

        size_t q = to.getcol();
        for (int j = 0; j < q; ++j)
        {
            float d = OUTPUT(m)[0][j] - y[0][j];
            c += d * d;
        }
    }
    return c / n;
}
void Xor_finite_diff(Xor &m, Xor &g, float eps, Matrix &ti, Matrix &to)
{
    float saved;
    float c = Xor_cost(m, ti, to);
    for (int i = 0; i < m.count; ++i)
    {
        for (int j = 0; j < m.ws[i].getrow(); ++j)
        {
            for (int k = 0; k < m.ws[i].getcol(); ++k)
            {
                saved = m.ws[i][j][k];
                m.ws[i][j][k] += eps;
                g.ws[i][j][k] = (Xor_cost(m, ti, to) - c) / eps;
                m.ws[i][j][k] = saved;
            }
        }

        for (int j = 0; j < m.bs[i].getrow(); ++j)
        {
            for (int k = 0; k < m.bs[i].getcol(); ++k)
            {
                saved = m.bs[i][j][k];
                m.bs[i][j][k] += eps;
                g.bs[i][j][k] = (Xor_cost(m, ti, to) - c) / eps;
                m.bs[i][j][k] = saved;
            }
        }
    }
}
void Xor_learn(Xor &m, Xor &g, float rate)
{
    for (int i = 0; i < m.count; ++i)
    {
        for (int j = 0; j < m.ws[i].getrow(); ++j)
        {
            for (int k = 0; k < m.ws[i].getcol(); ++k)
            {
                m.ws[i][j][k] -= rate * g.ws[i][j][k];
            }
        }

        for (int j = 0; j < m.bs[i].getrow(); ++j)
        {
            for (int k = 0; k < m.bs[i].getcol(); ++k)
            {
                m.bs[i][j][k] -= rate * g.bs[i][j][k];
            }
        }
    }
}