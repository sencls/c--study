/*
单例模式： 创建型设计模式，确保一个类在整个程序的生命周期中有且只有一个实例，并提供一个全局访问点来获取该实例
     1.控制实例数量：确保只有一个，放置创建多个而导致的资源浪费或状态不一致，例如：数据库连接池、配置管理类等
     2.提供全局访问点：使得程序在任何位置都可以方便的访问实例，对于需要在多模块或组件之间共享的资源或服务尤为重要，例如：日志记录器
                       缓存管理器
     3.延迟实例化： 通常采用懒加载模式。即第一次需要才创建，有助于节省系统资源，特别是在实例创建成本较高或初期并不需要该实例的情况下
     4.避免给命名冲突： 通过单例实例作为一个类的静态成员，避免在全局命名空间中引入多个实例，减少命名冲突的风险
     5.管理共享资源：在多线程模式下，单例可以有效管理共享资源，确保线程安全
   使用场景：1.需要确保全局只有一个实例，配置管理、日志系统、设配驱动/2.需要全局访问点来协调系统中的多个部分，缓存、线程池

   局部静态变量方式： c++98时存在隐患，多线程时可能生成多个，c++11编译器优化，避免了隐患(静态成员多次调用只会创建一次)
   class Single2
   {
    private:
        Single2() {}
        Single2(const Single2 &) = delete;
        Single2 &operator=(const Single2 &) = delete;

    public:
        static Single2 &GetInst()
        {
            static Single2 single;  //第一次调用 生成对象，后续调用共用
            return single;
        }
        ~Single2() { std::cout << "destory\n"; }
    };
 指针(饿汉式)  开始就创建 在对应的.cpp文件中或main()函数中： Single2Hungry* Single2Hungry::single =single2Hungry::GetInst();
    class Single2
    {
    private:
        Single2() {}
        Single2(const Single2 &) = delete;
        Single2 &operator=(const Single2 &) = delete;
        static Single2 *single;

    public:
        static Single2*GetInst()
        {
            if (single == nullptr)
            {
                single = new Single2();
            }
            return single;
        }
        ~Single2()
        {
            //delete single;
            std::cout << "destory\n";
        }
    };
    //析构存在问题
    指针(懒汉式)  调用时才创建
    class Single2
    {
    private:
        Single2() {}
        Single2(const Single2 &) = delete;
        Single2 &operator=(const Single2 &) = delete;
        static Single2 *single;
        static std::mutex _mtx;

    public:
        static Single2 *GetInst()
        {
            std::lock_guard<std::mutex> lock(_mtx);//存在串行的可能 改进：双重锁
            if (single == nullptr)
            {
                single = new Single2();
            }
            return single;
        }
        ~Single2()
        {
            std::cout << "destory\n";
        }
    };
       //存在串行的可能 改进：双重锁   //仍然存在线程不安全问题 ：因为new的关系，在多线程时可能取到开辟了地址但没有构造的结果，导致数据混乱或者直接崩溃
         static Single2 *GetInst()
        {
            if(single !=nullptr)
            {
                return single;
            }
            _mtx.lock();
            if (single != nullptr)
            {
                _mtx.unlock();
                single = new Single2();
            }
            single =new Single2();
            _mtx.unlock();
            return single;
        }

   //c++11 改进： once_flag
   class singleOnceFlag
{
private:
    singleOnceFlag() {}
    static std::shared_ptr<singleOnceFlag> _instance;

public:
    ~singleOnceFlag()
    {
        std::cout << "desorty\n";
    }
    singleOnceFlag &operator=(const singleOnceFlag &) = delete;
    singleOnceFlag(const singleOnceFlag &) = delete;

    static std::shared_ptr<singleOnceFlag> GetInst()
    {
        static std::once_flag flag;
        std::call_once(flag, []()
                       { _instance = std::shared_ptr<singleOnceFlag>(new singleOnceFlag()); });
        return _instance;               //为什么不用make_share() ->要调用构造函数，但是私有无法使用。看其原理
    }
};
std::shared_ptr<singleOnceFlag> singleOnceFlag::_instance = nullptr;//注意静态成员的类外初始化 ，链接时可能出错
//通过使用友元类来调用私有的析构函数->实现更极端的单例

//CRTP 通用的单例模板类 ：CRTP 将派生类作为模板参数传递给基类，即一个类继承自一个以自身为模板参数的基类，常用于静态多态、接口的默认实现
                          编译时策略选择
   template<typename T>
   class TempClass {};

   class RealClass :public TempClass<RealClass> {};

e.g.
template <typename T>
class Singleton

{
protected:
    Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    static std::shared_ptr<T> _instance;

public:
    ~Singleton() = default;
    static std::shared_ptr<T> GetInst()
    {
        static std::once_flag flag;
        std::call_once(flag, []()
                       { _instance = std::shared_ptr<T>(new T()); });
        return _instance;
    }
};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

class SingleNet : public Singleton<SingleNet>
{
    friend Singleton<SingleNet>;

private:
    SingleNet() = default;

public:
    ~SingleNet() { std::cout << "destory\n"; }
};
*/
#include "sp.h"
#include <iostream>
#include <thread>
#include <mutex>
int main()
{
    std::cout << SingleNet::GetInst() << std::endl;
    std::thread t1([]()
                   { std::cout << SingleNet::GetInst() << std::endl; });
    std::thread t2([]()
                   { std::cout << SingleNet::GetInst() << std::endl; });

    t1.join();
    t2.join();
    return 0;
}