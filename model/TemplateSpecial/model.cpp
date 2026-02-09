#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "Logger.h"
/*template <template <typename, typename> class node, typename R>//模板模板化
class add
{
    void print(const node<R, std::allocator<R>> &c)
    {
        for (auto &t : c)
        {
            std::cout << t << std::endl;
        }
    }
        add<std::vector, int> ma;
};
//模板特化：1.全特化，2.偏特化  函数只能全特化 ，编译器 偏特化>全特化>通用
template <typename a，typename b>
class text
{
public:
    void print()
    {
        std::cout << "我一样";
    }
};
template <>
class text<std::string,int> //全特化
{
public:
    void print()
    {
        std::cout << "我不一样";
    }
};
template <typename a，typename b>
class text<a,int*> //偏特化
{
public:
    void print()
    {
        std::cout << "我不一样";
    }
};

template<typename a>
void print(const a& tri) {}

template<>
void print<int*>(int* const& value) {}

//变参模板 : 使用参数包，通过...语法来表示
template <typename... Args>
class as {};
template <typename T, typename... Args>
void myfun(T first, Args... args) { std::cout << first; }

e.g.void printl() { std::cout << "all"; }
    template <typename a, typename... Args>
    void printl(const a &first, const Args &...args)--->int a = 1, b = 1, c = 1, d = 1, e = 1, f = 1;
    {                                                  printl(a, b, c, d, e, f);
        std::cout << first << " ";
        printl(args...);
    }
c++17优化：折叠表达式：
    template <typename... Args>
    void ip(const Args &...args)
    {

       ((std::cout << args), ...);
    }

    折叠表达式： 一元：对内部参数使用一个一元操作符，二元(左折叠，右折叠)：使用二元操作符
    一元： (op...pack),(pack...op) //不常用
    bool sum(const Args &...args)
    {
        return (!args && ...);     //取非，再与；  (!a)&&(!b)&&...
    }
    二元:   左折叠(init op ... op pack),从左到右结合
            右折叠(pack op ... op init op ...)
    e.g.   左折叠：auto sum(const Args&... args) ->decltype((args * ...))
           {
               return (args+...)
           }
           右折叠 auto sum(const Args &...args) -> decltype((... * args))
                {
                    return (... * args);
                }
            void print(const Args &...args)
            {
                //((std::cout << args), ...);
                //(std::cout << ... << args);
            }
            template <typename... Args, typename a>
            auto maxn(const a &t, const Args &...args) -> decltype(std::max(t, (args, ...)))
            {
                return std::max(t, (args, ...));
            }
            template <typename... Args, typename a>
            auto maxn(const a &t, const Args &...args) -> decltype(std::max(t, (args, ...)))
            {
                return std::max(t, (args, ...));
            }
//SFINAE (Substitution is not an error):替换失败不是错误，编译器实例化时，在替换模板参数时失败，不会将其视作编译错误
        而是寻找其他可能的模板或者重载
    应用：1.函数重载选择 2.类型特性检测 3.条件编译
e.g. 使用enable_if(condition，return_typee)来确定版本  ，结合<type_traits>来对类型进行确定；condition为true时，编译函数，并
    template <typename T>                                                     给出函数的返回类型
    typename std::enable_if<std::is_integral<T>::value, void>::type
    print_type(T value)
    {
        std::cout << "int " << std::endl;
    }
    typename std::enable_if<std::is_same<T,const char*>::value, void>::type
    is_point<>::value 确定是否指针

c++20 ： concept Integral =std::is_integral_v<T>; ,requires() 搭配使用，前者限定条件，后者排除条件
   e.g.  template <typename T>
         concept printable = requires(T a) {
         { std::cout << a } -> std::same_as<std::ostream &>;
        };
         template <printable T>
         void print(T a)
         {
             std::cout << a << std::endl;
          }

   2.判断是否含有该成员
    e.g. template <typename T>
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




*/
int main()
{
    int a = 10;
    std::string str = "qwe";
    Logger<int *>::log(&a);
    Logger<std::string>::log(str);
    logger(a);
    logger(str);
    loggerAll(a, str, &a);
    return 0;
}