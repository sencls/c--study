/*
左值与右值
    lvalue: 具有持久存储的对象，可出现在赋值语句左侧，可被取地址，如：变量名，引用
    rvalue： 临时对象或没有持久存储的值，通常出现在赋值语句右侧，不能被取地址，如： 字面量，临时对象，表达式结果

    c++11 ：rvalue： 纯右值Prvalue：临时对象，字面量
                    将亡值xvalue，expiring value：表示即将被移动的对象，std::move()的结果
    std::is_lvalue_reference<decltype()>::value 判断是否是左值引用
    std::is_lvalue_reference_v<decltype()>     //前两者判断左值时  decltype((a))->两层括号

    int a = 10;// 右值        std::is_lvalue_reference_v<decltype((a))> -true;
    int &b = a;             std::is_lvalue_reference_v<decltype(b)> -true；
模板万能引用(转发引用)：forwarding reference c++11引入的引用类型
   1.表现形式为 T&& ：T是模板参数
   2.编译器会解析其为左值引用还是右值引用    // T被推导为普通类型，右值引用;引用类型，左值引用
   3.能统一处理左值与右值

   //引用折叠规则：处理引用类型的嵌套
    e.g. &&、& &&、&& & ->& 左值引用：只要有左值引用 就是左值引用
         && && ->右值引用：只要是右值引用就是右值引用

    //右值引用类型做表达式时->左值
    template <typename T>
    void fun1(T &&x)
    {
        std::
        std::cout << "lv " << std::is_lvalue_reference_v<T> << std::endl;
        std::cout << "rv " << std::is_rvalue_reference_v<T> << std::endl;
        std::cout << "x is rv " << std::is_rvalue_reference_v<T &&> << std::endl;
        std::cout << "x is lv " << std::is_lvalue_reference_v<decltype(x)> << std::endl;
        std::cout << "x is rv " << std::is_rvalue_reference_v<decltype(x)> << std::endl;
    }
    fun1(a);
    fun1(c);
    fun1(std::move(c));
  //重载函数 ，通过std::forward<type>() 来保持类型；
    void process(int &x)
{
    std::cout << "(int&)" << std::endl;
}
void process(int &&x)
{
    std::cout << "(int&&)" << std::endl;
}
template <typename T>
void wrapper(T &&x)
{
    process(std::forward<T>(x));
}

void display_type(T &&param)
{
    std::cout << "T is lv reference: " << std::is_lvalue_reference_v<T> << std::endl;
    std::cout << "param is lvalue reference: " << std::is_lvalue_reference_v<decltype(param)> << std::endl;
    std::cout << "T traits normal type is int " << std::is_integral_v<typename std::remove_reference_t<T>> << std::endl;
}

//类型推导规则： 模板编程中，编译器根据传入的实参自动推导模板参数类型
      1.当模板参数与函数参数匹配时，自动推导
      2.引用的处理： 引用折叠
      3.cv限定符的去除；const与volatile修饰会去除
     //std::remove_reference_t<> 去除引用

//原样转发： 在模板函数中，将接收到的参数以其原有的值类别(lv,rv)传递给另一个函数，确保泛型代码可以正确处理参数
    1.保持参数值类别不变，确保正确调用函数重载
    2.利用移动语义，避免不必要的拷贝，提升性能
    3.编写通用、复用性强的代码
    通过 模板万能引用，std::forward<T>()实现；

    template <typename F, typename T1, typename T2>
void flip1(F f, T1 t1, T2 t2)
{
    f(t2, t1);
}
template <typename F, typename T1, typename T2>
void flip2(F f, T1 &&t1, T2 &&t2)
{
    f(t2, t1);
}
void ftemp(int v1, int &v2)
{
    std::cout << v1 << ' ' << ++v2 << std::endl;
}
void use_ftemp1()
{
    int i = 100, j = 99;
    flip1(ftemp, j, 42);
    std::cout << j << std::endl;
    std::cout << i << std::endl;
}
void use_ftemp2()
{
    int i = 100, j = 99;
    flip2(ftemp, j, 42);
    std::cout << j << std::endl;
    std::cout << i << std::endl;
}
void gtemp(int &&i, int &j)
{
    std::cout << i << ' ' << ++j << std::endl;
}
void use_ftemp3()
{
    int i = 100, j = 99;
    flip2(gtemp, i, j);
    std::cout << j << std::endl;
    std::cout << i << std::endl;
}

*/
#include <iostream>
#include <utility>
#include <type_traits>
int main()
{

    return 0;
}