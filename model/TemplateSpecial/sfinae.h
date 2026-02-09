#pragma once

#include <iostream>
#include <type_traits>
#include <string>
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
print_type(T value)
{
    std::cout << "int " << std::endl;
}

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, void>::type
print_type(T value)
{
    std::cout << "string" << std::endl;
}

template <typename T>
concept Integral = std::is_integral_v<T>;

void cout_int(Integral auto value)
{
    std::cout << "int" << std::endl;
}

template <typename T>
    requires(!Integral<T>)
void cout_int(T value)
{
    std::cout << "int" << std::endl;
}

template <typename T>
class has_foo
{
private:
    typedef char yes[1];
    typedef char no[2];
    template <typename U, void (U::*)()> // u::* u的成员函数指针
    struct SFINAE
    {
    };

    template <typename U>
    static yes &test(SFINAE<U, &U::foo> *);

    template <typename U>
    static no &test(...); //...指任意参数
public:
    static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
};
template <typename T>
typename std::enable_if<has_foo<T>::value, void>::type
call_foo(T &obj)
{
    obj.foo();
    std::cout << "foo()" << std::endl;
}

class Withfoo
{
public:
    void foo() { std::cout << "has\n"; }
};
class nofoo
{
};

template <typename T>
concept printable = requires(T a) {
    { std::cout << a } -> std::same_as<std::ostream &>;
};
template <printable T>
void print(T a)
{
    std::cout << a << std::endl;
}

template <typename T, typename = void>
struct has_non_void_value_type : std::false_type
{
};

template <typename T>
struct has_non_void_value_type<T, std::__enable_if_t<!std::is_void_v<typename T::value_type>>> : std::true_type
{
};
template <typename T, bool HasValueType = has_non_void_value_type<T>::value>
struct TypePrint
{
};
template <typename T>     // T有成员类型
struct TypePrint<T, true> // 显式添加typename是为了告诉编译器这是类型，而不是对象
{                         // T::value_type ->T的类型 ，vecto<int>::value_type =int
    static void print()
    {
        std::cout << "T has a member type value_type";
    }
};
template <typename T>
struct TypePrint<T, false>
{
    static void print()
    {
        std::cout << "no type";
    }
};

struct Withint
{
    using value_type = int;
};

struct noint
{
};
