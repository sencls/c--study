/*
// 模板元编程：在编译期通过模板机制进行代码生成与计算的技术，利用编译器的模板实例化机制，在编译期执行代码逻辑，提高性能和灵活性
//     优点： 提高代码的可重复性与泛化能力；减小运行时开销；实现类型安全的高级抽象
//     基础 ： 模板特化，递归模板
e.g.template <int N>
    struct Factorial
    {
         static const int value = N * Factorial<N - 1>::value;//改为inline static const int value = N * Factorial<N - 1>::value
     };                                                           就不需要类外定义，来防止取内存崩；
     template <>
     struct Factorial<0>
     {
         static const int value = 1;
     };
     template <int N>
     const int Factorial<N>::value;

     // 为 Factorial<0>::value 提供定义，使其可以取地址
     template <>
     const int Factorial<0>::value;    //静态编译会报错，但不影响运行

   //实现编译阶段，判断编译类型是否具有一定能力
    template <typename T, typename = void>
    struct is_addable : std::false_type
    {
    };

    template <typename T>
    struct is_addable<T, decltype(void(std::declval<T>() + std::declval<T>()))> : std::true_type
    {

    }; // declval :不创建实际对象，但返回一个对应类型的右值引用

    static_assert(is_addable<int>::value, "int should be added");
    static_assert(is_addable<void>::value, "no");
    //类型检测
    template <typename... Ts>
struct typelist
{
};
template <typename list, std::size_t N>
struct TypeAt;

template <typename Head, typename... Tail>
struct TypeAt<typelist<Head, Tail...>, 0>
{
    using type = Head;
};

template <typename Head, typename... Tail, std::size_t N>
struct TypeAt<typelist<Head, Tail...>, N>
{
    using type = typename TypeAt<typelist<Tail...>, N - 1>::type;
};

*/
#include "template.h"
#include <iostream>

int main()
{
    int ans = sum<1, 2, 3, 4, 5>::value;
    std::cout << ans;
    std::cout << Fibonacci<5>::value << std::endl;
    using list = typelist<int, double, char>;
    using third = TypeAt<list, 1>::type;
    std::cout << typeid(third).name() << std::endl;
    return 0;
}