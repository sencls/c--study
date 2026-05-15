#include "Matrix.h"

int main()
{
    srand(time(0));
    Matrix a(1, 2, 10);
    Matrix b(2, 2, 4);
    Matrix rt = a * b;

    mat_print(rt);
    return 0;
}