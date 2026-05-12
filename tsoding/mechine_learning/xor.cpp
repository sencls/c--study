#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

using sample = vector<vector<float>>;

sample xor_train = {{0, 0, 0}, {1, 0, 1}, {0, 1, 1}, {1, 1, 0}};
sample and_train = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 1}};
sample or_train = {{0, 0, 0}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
sample nand_train = {{0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 0}};
float rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}
struct XOR
{
    float or_w1;
    float or_w2;
    float or_b;
    float nand_w1;
    float nand_w2;
    float nand_b;
    float and_w1;
    float and_w2;
    float and_b;
    void println()
    {
        cout << "or_w1 = " << or_w1 << '\n'
             << "or_w2 = " << or_w2 << '\n'
             << "or_b = " << or_b << '\n'
             << "nand_w1 = " << nand_w1 << '\n'
             << "nand_w2 = " << nand_w2 << '\n'
             << "nand_b = " << nand_b << '\n'
             << "and_w1 = " << and_w1 << '\n'
             << "and_w2 = " << and_w2 << '\n'
             << "and_b = " << and_b << endl;
    }
};
float sigmoid(float x)
{
    return 1.f / (1.f + exp(-x));
}
float forward(XOR m, float x1, float x2)
{
    float a = sigmoid(m.or_w1 * x1 + m.or_w2 * x2 + m.or_b);
    float b = sigmoid(m.nand_w1 * x1 + m.nand_w2 * x2 + m.nand_b);
    return sigmoid(m.and_w1 * a + m.and_w2 * b + m.and_b);
}

sample &train = and_train;
size_t train_count = 4;

float cost(XOR m)
{
    float result = 0.0f;
    for (int i = 0; i < train_count; ++i)
    {
        float x1 = train[i][0];
        float x2 = train[i][1];

        float y = forward(m, x1, x2);
        float d = y - train[i][2];
        result += d * d;
    }
    result /= train_count;
    return result;
}

XOR rand_xor()
{
    XOR m;
    m.or_w1 = rand_float();
    m.or_w2 = rand_float();
    m.or_b = rand_float();
    m.nand_w1 = rand_float();
    m.nand_w2 = rand_float();
    m.nand_b = rand_float();
    m.and_w1 = rand_float();
    m.and_w2 = rand_float();
    m.and_b = rand_float();
    return m;
}

void print_xor(XOR m)
{
    m.println();
}

XOR finite_diff(XOR m, float eps)
{
    XOR g;
    float c = cost(m);
    float saved;

    saved = m.or_w1;
    m.or_w1 += eps;
    g.or_w1 = (cost(m) - c) / eps;
    m.or_w1 = saved;

    saved = m.or_w2;
    m.or_w2 += eps;
    g.or_w2 = (cost(m) - c) / eps;
    m.or_w2 = saved;

    saved = m.or_b;
    m.or_b += eps;
    g.or_b = (cost(m) - c) / eps;
    m.or_b = saved;

    saved = m.nand_w1;
    m.nand_w1 += eps;
    g.nand_w1 = (cost(m) - c) / eps;
    m.nand_w1 = saved;

    saved = m.nand_w2;
    m.nand_w2 += eps;
    g.nand_w2 = (cost(m) - c) / eps;
    m.nand_w2 = saved;

    saved = m.nand_b;
    m.nand_b += eps;
    g.nand_b = (cost(m) - c) / eps;
    m.nand_b = saved;

    saved = m.and_w1;
    m.and_w1 += eps;
    g.and_w1 = (cost(m) - c) / eps;
    m.and_w1 = saved;

    saved = m.and_w2;
    m.and_w2 += eps;
    g.and_w2 = (cost(m) - c) / eps;
    m.and_w2 = saved;

    saved = m.and_b;
    m.and_b += eps;
    g.and_b = (cost(m) - c) / eps;
    m.and_b = saved;
    return g;
}

XOR learn(XOR m, XOR g, float rate)
{
    m.or_w1 -= rate * g.or_w1;
    m.or_w2 -= rate * g.or_w2;
    m.or_b -= rate * g.or_b;
    m.nand_w1 -= rate * g.nand_w1;
    m.nand_w2 -= rate * g.nand_w2;
    m.nand_b -= rate * g.nand_b;
    m.and_w1 -= rate * g.and_w1;
    m.and_w2 -= rate * g.and_w2;
    m.and_b -= rate * g.and_b;
    return m;
}
float and_b;

int main()
{
    XOR m = rand_xor();
    const float eps = 1e-1, rate = 1e-1;
    for (int i = 0; i < 100000; ++i)
    {
        XOR g = finite_diff(m, eps);
        cout << cost(m) << ' ';
        m = learn(m, g, rate);
        cout << cost(m) << endl;
    }
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            cout << i << '^' << j << ' ' << forward(m, i, j) << endl;
        }
    }
    return 0;
}