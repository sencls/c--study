#include "Matrix.h"

#define BITS 2

int main()
{

    size_t n = 1 << BITS;
    size_t rows = n * n;
    Matrix ti(rows, BITS + BITS);
    Matrix to(rows, BITS + 1);
    for (size_t i = 0; i < ti.getrow(); ++i)
    {
        size_t x = i / n;
        size_t y = i % n;
        size_t z = x + y;
        // size_t overflow = z >= n;
        for (size_t j = 0; j < BITS; ++j)
        {
            ti[i][j] = (x >> j) & 1;
            ti[i][j + BITS] = (y >> j) & 1;
            to[i][j] = (z >> j) & 1;
            // if (overflow)
            // {
            //     ti[i][j] = 0;
            //     ti[i][j + BITS] = 0;
            //     to[i][j] = 0;
            // }
            // else
            // {

            //     to[i][j] = (z >> j) & 1;
            // }
        }
        to[i][BITS] = z >= n;
    }
    mat_printn(ti);
    mat_printn(to);
    float rate = 1;
    std::vector<size_t> arch{2 * BITS, 2 * BITS + 1, BITS + 1};
    NetWork m = create(arch, arch.size()), g = create(arch, arch.size());
    random(m, 0, 1);
    std::cout << cost(m, ti, to) << std::endl;
    for (int i = 0; i < 5000; ++i)
    {
        back_prop(m, g, ti, to);
        learn(m, g, rate);
        std::cout << cost(m, ti, to) << std::endl;
    }

    for (size_t x = 0; x < n; ++x)
    {
        for (size_t y = 0; y < n; ++y)
        {
            std::cout << x << "+" << y << " = ";
            for (size_t j = 0; j < BITS; ++j)
            {
                INPUT(m)
                [0][j] = (x >> j) & 1;
                INPUT(m)
                [0][j + BITS] = (y >> j) & 1;
            }
            forward(m);
            if (OUTPUT(m)[0][BITS] > 0.5f)
            {
                std::cout << "OVERFLOW\n";
            }
            else
            {
                size_t z = 0;
                for (size_t j = 0; j < BITS; ++j)
                {
                    size_t bit = OUTPUT(m)[0][j] > 0.5f;
                    z |= bit << j;
                }
                std::cout << z << std::endl;
            }
        }
    }
    return 0;
}