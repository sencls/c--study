#pragma once
#include <type_traits>
template <int N>
struct Factorial
{
    static const int value = N * Factorial<N - 1>::value;
};
template <>
struct Factorial<0>
{
    static const int value = 1;
};

template <int N>
struct Fibonacci
{
    inline static const int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template <>
struct Fibonacci<0>
{
    inline static const int value = 0;
};
template <>
struct Fibonacci<1>
{
    inline static const int value = 1;
};

template <typename T, typename = void>
struct is_addable : std::false_type
{
};

template <typename T>
struct is_addable<T, decltype(void(std::declval<T>() + std::declval<T>()))> : std::true_type
{

}; // declval :不创建实际对象，但返回一个对应类型的右值引用

template <int... Ns>
struct sum;

template <>
struct sum<>
{
    static const int value = 0;
};

template <int N, int... NS>
struct sum<N, NS...>
{
    static const int value = N + sum<NS...>::value;
};
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