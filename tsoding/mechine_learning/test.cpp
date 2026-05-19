#include "Matrix.h"
#include <iostream>

// void forward_xor(Xor &m)
// {

//     m.a1 = m.a0 * m.w1 + m.b1;
//     mat_sig(m.a1);
//     m.a2 = m.a1 * m.w2 + m.b2;
//     mat_sig(m.a2);
// }
// float cost(Xor &m, Matrix &ti, Matrix &to)
// {
//     assert(ti.getrow() == to.getrow());
//     assert(to.getcol() == m.a2.getcol());
//     size_t n = ti.getrow();
//     float c = 0;
//     for (int i = 0; i < n; ++i)
//     {

//         Matrix x = mat_row(ti, i);

//         Matrix y = mat_row(to, i);

//         mat_copy(m.a0, x);

//         forward_xor(m);

//         size_t q = to.getcol();
//         for (int j = 0; j < q; ++j)
//         {
//             float d = m.a2[0][j] - y[0][j];
//             c += d * d;
//         }
//     }
//     return c / n;
// }
// void setXor(Xor &m)
// {
//     mat_size(m.a0, 1, 2);
//     mat_size(m.w1, 2, 2);
//     mat_size(m.b1, 1, 2);
//     mat_size(m.a1, 1, 2);
//     mat_size(m.w2, 2, 1);
//     mat_size(m.b2, 1, 1);
//     mat_size(m.a2, 1, 1);
// }
// Matrix td{4, 3, {{0, 0, 0}, {0, 1, 1}, {1, 0, 1}, {1, 1, 0}}};
// void finite_diff(Xor &m, Xor &g, float eps, Matrix ti, Matrix to)
// {
//     float saved;
//     float c = cost(m, ti, to);

//     for (int i = 0; i < m.w1.getrow(); ++i)
//     {
//         for (int j = 0; j < m.w1.getcol(); ++j)
//         {
//             saved = m.w1[i][j];
//             m.w1[i][j] += eps;
//             g.w1[i][j] = (cost(m, ti, to) - c) / eps;
//             m.w1[i][j] = saved;
//         }
//     }
//     for (int i = 0; i < m.b1.getrow(); ++i)
//     {
//         for (int j = 0; j < m.b1.getcol(); ++j)
//         {
//             saved = m.b1[i][j];
//             m.b1[i][j] += eps;
//             g.b1[i][j] = (cost(m, ti, to) - c) / eps;
//             m.b1[i][j] = saved;
//         }
//     }
//     for (int i = 0; i < m.w2.getrow(); ++i)
//     {
//         for (int j = 0; j < m.w2.getcol(); ++j)
//         {
//             saved = m.w2[i][j];
//             m.w2[i][j] += eps;
//             g.w2[i][j] = (cost(m, ti, to) - c) / eps;
//             m.w2[i][j] = saved;
//         }
//     }
//     for (int i = 0; i < m.b2.getrow(); ++i)
//     {
//         for (int j = 0; j < m.b2.getcol(); ++j)
//         {
//             saved = m.b2[i][j];
//             m.b2[i][j] += eps;
//             g.b2[i][j] = (cost(m, ti, to) - c) / eps;
//             m.b2[i][j] = saved;
//         }
//     }
// }
// void Xor_learn(Xor &m, Xor &g, float rate)
// {
//     for (int i = 0; i < m.w1.getrow(); ++i)
//     {
//         for (int j = 0; j < m.w1.getcol(); ++j)
//         {
//             m.w1[i][j] -= rate * g.w1[i][j];
//         }
//     }
//     for (int i = 0; i < m.w2.getrow(); ++i)
//     {
//         for (int j = 0; j < m.w2.getcol(); ++j)
//         {
//             m.w2[i][j] -= rate * g.w2[i][j];
//         }
//     }
//     for (int i = 0; i < m.b1.getrow(); ++i)
//     {
//         for (int j = 0; j < m.b1.getcol(); ++j)
//         {
//             m.b1[i][j] -= rate * g.b1[i][j];
//         }
//     }
//     for (int i = 0; i < m.b2.getrow(); ++i)
//     {
//         for (int j = 0; j < m.b2.getcol(); ++j)
//         {
//             m.b2[i][j] -= rate * g.b2[i][j];
//         }
//     }
// }
int main()
{
    // srand(time(0));
    // Xor m, g;
    // setXor(m);
    // setXor(g);
    // Matrix ti = mat_sub(td, 4, 2);
    // Matrix to = mat_sub(td, 4, 1, 0, 2);

    // mat_rand(m.w1, 0, 1);
    // mat_rand(m.b1, 0, 1);
    // mat_rand(m.w2, 0, 1);
    // mat_rand(m.b2, 0, 1);

    // float eps = 1e-1, rate = 1e-1;
    // std::cout << cost(m, ti, to) << std::endl;
    // for (int i = 0; i < 11000; i++)
    // {
    //     finite_diff(m, g, eps, ti, to);
    //     Xor_learn(m, g, rate);
    //     std::cout << cost(m, ti, to) << std::endl;
    // }

    // for (int i = 0; i < 2; ++i)
    // {
    //     for (int j = 0; j < 2; ++j)
    //     {
    //         m.a0[0][0] = i;
    //         m.a0[0][1] = j;
    //         forward_xor(m);
    //         float y = m.a2[0][0];
    //         std::cout << i << ' ' << j << " " << y << std::endl;
    //     }
    // }
    srand(time(0));
    std::vector<size_t> layers{2, 2, 1};
    Xor m = create(layers, layers.size());
    Xor_random(m, 0, 1);
    print_xorn(m);
    return 0;
}