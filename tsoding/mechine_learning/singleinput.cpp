#include <iostream>
#include <cmath>
#include <random>
#include <cmath>
using namespace std;
#define train_count (sizeof(train) / sizeof(train[0]))
float rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}

float train[][2] = {
    {0.0f, 0.0f}, {1.0f, 2.0f}, {2.0f, 4.0f}, {3.0f, 6.0f}, {4.0f, 8.0f}};

float cost(float w, float b)
{
    float result = 0.0f;
    for (int i = 0; i < train_count; ++i)
    {
        float x = train[i][0];
        float y = x * w + b;
        float d = y - train[i][1];
        result += d * d;
    }
    result /= train_count;
    return result;
}

int main()
{
    // model is y=x*w;
    srand(69);
    float w = rand_float() * 10.0f;
    float b = rand_float() * 5.0f;
    cout << cost(w, b) << endl;

    float eps = 1e-3;
    float rate = 1e-3;
    for (int i = 0; i < 5000; ++i)
    {
        float dw = (cost(w + eps, b) - cost(w, b)) / eps;
        float db = (cost(w, b + eps) - cost(w, b)) / eps;
        w -= rate * dw;
        b -= rate * db;
        cout << "cost: " << cost(w, b) << " w: " << w << " b: " << b << endl;
    }
    return 0;
}