#include "Matrix.h"

int main()
{
    srand(time(0));
    Matrix x(1, 2);

    x[0][0] = 0;
    x[0][1] = 1;

    Matrix w1(2, 2);
    Matrix b1(1, 2);
    Matrix a1(1, 2);

    a1 = x * w1 + b1;
    mat_sig(a1);

    Matrix w2(2, 1);
    Matrix b2(1, 1);

    mat_rand(w1, 0, 1);
    mat_rand(w2, 0, 1);
    mat_rand(b1, 0, 1);
    mat_rand(b2, 0, 1);

    mat_printn(x);
    mat_printn(w1);
    mat_printn(w2);
    mat_printn(b1);
    mat_printn(b2);

    return 0;
}