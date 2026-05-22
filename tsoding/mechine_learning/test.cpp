#include "Matrix.h"
#include <iostream>

Matrix td_xor{4, 3, {{0, 0, 0}, {0, 1, 1}, {1, 0, 1}, {1, 1, 0}}};
Matrix td_or{4, 3, {{0, 0, 0}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}}};
Matrix td_and{4, 3, {{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 1}}};
Matrix td_nand{4, 3, {{0, 0, 1}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}}};
Matrix *td = &td_xor;

int main()
{

    Matrix ti = mat_sub(*td, 4, 2);
    Matrix to = mat_sub(*td, 4, 1, 0, 2);

    float eps = 1e-1, rate = 1e-1;

    srand(69);
    std::vector<size_t> layers{2, 2, 1};
    Xor m = create(layers, layers.size()), g = create(layers, layers.size());
    Xor_random(m, 0, 1);

    std::cout << Xor_cost(m, ti, to) << std::endl;
    for (size_t i = 0; i < 10000; ++i)
    {
        // Xor_finite_diff(m, g, eps, ti, to);

        back_prop(m, g, ti, to);
        Xor_learn(m, g, rate);
        std::cout << Xor_cost(m, ti, to) << std::endl;
    }

    print_xorn(m);

    std::cout << "----------------------------------------\n";
    for (size_t i = 0; i < 2; ++i)
    {
        for (size_t j = 0; j < 2; ++j)
        {
            INPUT(m)
            [0][0] = i;
            INPUT(m)
            [0][1] = j;
            Xor_forward(m);
            std::cout << i << "^" << j << " " << OUTPUT(m)[0][0] << std::endl;
        }
    }
    return 0;
}