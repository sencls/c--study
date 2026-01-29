/*可调用对象：函数指针，仿函数，lambda表达式，std::function,std::bind

//函数指针                                          //优点：简单直观，适合简单的回调函数
    int (*p)(int, int);                              缺点：  不能捕获上下文，指针的声明和使用较复杂
    int add(int a, int b)
    {
        return a + b;
    }
    p = add;                 // p=&add;
    std::cout << (*p)(1, 2); // p(1,2)


//仿函数(函数对象)，是重载了operate()的类或结构体
    //简单应用                                                //优点：携带状态：可以拥有内部状态，通过成员变量存储数据，调用时具备上下文
    struct adder                                                灵活性高：可以根据需要添加更多的成员函数和变量，拓展功能
    {                                                           性能优化： 编译器对仿函数优化，内联展开
        int to_add;                                             类型安全： 使用时编译期间检查
        adder(int val) : to_add(val) {}                         使用： 需要携带状态时，复杂操作时(lambda难以表达的复杂逻辑)，性能关键场景
        int operator()(int x)
        {
            return x + to_add;
        }
    };
    adder a(2);
    std::cout << a.to_add;
    int b = a(1);
    std::cout << b;

    //同stl使用
    struct IsGreatThan
    {
        int threshold;
        IsGreatThan(int val):threshold(val) {}
        bool operator()(int x) const
        {
            return x>threshold;
        }
    }

    IsGreatThan great(10);
    std::vector<int> num = {1, 2, 10, 7};
    auto it = std::find_if(num.begin(), num.end(), great);
    if (it != num.end())
        std::cout << *it;
    else
        std::cout << "None";

    //同模版使用;泛型比较器
    template <typename T>
    struct Compare
    {
        bool operator()(const T&a,const T&b) const
        {
            return a<b;
        }
    }
    std::vector<int> a = {1, 1, 1, 44565, 3, 7, 8, 312};
    std::sort(a.begin(), a.end(), Compare<int>());
    for (const auto &t : a)
    {
        std::cout << t << " ";
    }
//lambda表达式 [capture](parameters)->return_type {//函数体}
    capture向外捕获需要的变量，值捕获，引用捕获，混合捕获-->  [=]值捕获，[&]引用捕获，[=,&val],[&,val]混合捕获,
                                                         使用时[=name]加变量名只收集对应的内容，不加会收集前面所有变量的
    parameters 参数列表
    return_type 可以不写，c++14自动推导
    //通过erase与remove_if实现目标删除                                         []() mutable 可变lambda 可以值捕获的变量在函数内修改，但作用域外的值不会修改
        int threshold = 10;
        std::vector<int> a = {1, 12, 111, 4, 3, 7, 8, 312};
        a.erase(std::remove_if(a.begin(), a.end(),
                               [threshold](int n) -> bool //true满足条件删除              优点： 简洁，灵活，性能优化，与stl无缝结合使用
                                {
                                   return n > threshold;
                                }),
                                a.end());
    //参数使用
    int a=19;
    int *temp=&a;
    auto la = [temp](int x) -> void
    {
        *temp *= x;
    };
    la(2);
    //类中使用
    class Precessor
    {
        int _threshold;

    public:
        Precessor(int threshold) : _threshold(threshold) {}
        void process(std::vector<int> &data)
        {
            for (const auto &t : data)
                std::cout << t << ' ';
            std::cout << std::endl;
            data.erase(std::remove_if(data.begin(), data.end(),
                                  [this](int x)
                                      -> bool
                                  { return x <= _threshold; }),
                   data.end());
            for (const auto &t : data)
                std::cout << t << ' ';
        }
    };
     auto dfun = [&a](int x) -> void //类外使用a是类的变量，用引用捕获
    { a.m(23, x); };
//std::fuction(functional库) c++11通用的可调用对象，可以封装任何可以调用对象，包括普通函数，lambda函数，函数对象以及绑定表达式

    std::function<int(int, int)> callback = add; //绑函数
    std::cout << callback(1, 2) << std::endl;
    A a;
    std::function<int(int, int)> fun1 = a;   //仿函数                                   特点： 可以存储各种可调用对象，支持动态改变存储的可调用对象，
    std::cout << fun1(1, 2) << std::endl;                                                     频繁调用时开销
    std::function<int(int)> fun2 = [](int x) -> int  //lambda函数
    { return x; };
    std::cout << fun2(3) << std::endl;
    std::function<void()> fun3 = []() -> void
    {
        std::cout << "aaaaa" << std::endl;
    };
 使用场景
    //回调函数
    using CallBack = std::function<void(int)>;
    void triggerEvent(CallBack cb, int value)
    {
        cb(value);
    }
    triggerEvent([](int x) -> void
                 { std::cout << "aaa" << ' ' << x << std::endl; }, 3);
    struct Printer
    {
        void operator()(int x) const
        {
            std::cout << "调用" << x << std::endl;
        }
    } printt;
    triggerEvent(printt, 4);

    //存储与调用不同类型的可调用对象
      使用容器类来存储,比如 vector<std::function<int(int,int)>>,map之类
      emplace_back比push_back少一次构造，更节省;

//std::bind(),在functional库中，可以绑定对象函数，同时控制一定的参数，待定参数使用std::placeholders占位
      bind(绑定对象，参数1，参数2)
    auto neew = std::bind(add, std::placeholders::_1, 2)==add(x,2);//函数
     auto neew = std::bind(&A::m, &a, std::placeholders::_1, 2); //类成员函数，成员函数需要取地址，类对象做第二个参数，也取地址
                                                                    静态成员函数不需要第二参数
*/
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
class A
{
public:
    A() : res(0) {}
    int res;
    int m(int a, int b)
    {
        res = a * b;
        return res;
    }
};
int main()
{
    A a;
    auto neew = std::bind(&A::m, &a, std::placeholders::_1, 2);
    auto dfun = [&a](int x) -> int
    { return a.m(23, x); };
    std::cout << neew(3) << ' ' << dfun(1);

    return 0;
}