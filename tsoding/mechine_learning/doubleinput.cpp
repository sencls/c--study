#include <iostream>
#include <cmath>
#include <random>
#include <cmath>
using namespace std;
#define train_count (sizeof(or_train) / sizeof(or_train[0]))
float rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}
float sigmoid(float x)
{
    return 1.f / (1.f + exp(-x));
}

// 双输入神经元

float or_train[][3] = {{0, 0, 0}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}}; // 或门
float and_train[][3] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 1}};
float nand_train[][3] = {{0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 0}};

float cost(float w1, float w2, float b)
{
    float result = 0.0f;
    for (int i = 0; i < train_count; ++i)
    {
        float x1 = or_train[i][0];
        float x2 = or_train[i][1];

        float y = sigmoid(x1 * w1 + x2 * w2 + b);
        float d = y - or_train[i][2];
        result += d * d;
    }
    result /= train_count;
    return result;
}

int main()
{
    srand(69);
    float w1 = rand_float();
    float w2 = rand_float();
    float b = rand_float();
    float eps = 1e-1, rate = 1e-1;

    for (int i = 0; i < 100000; ++i)
    {
        float c = cost(w1, w2, b);
        cout << w1 << ' ' << w2 << ' ' << b << ' ' << c << endl;
        float dw1 = (cost(w1 + eps, w2, b) - c) / eps;
        float dw2 = (cost(w1, w2 + eps, b) - c) / eps;
        float db = (cost(w1, w2, b + eps) - c) / eps;
        w1 -= rate * dw1;
        w2 -= rate * dw2;
        b -= rate * db;
    }

    return 0;
}