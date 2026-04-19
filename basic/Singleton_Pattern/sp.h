#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <utility>
#include <memory>
class Single2
{
private:
    Single2() {}

    static Single2 *single;
    static std::mutex _mtx;

public:
    Single2(const Single2 &) = delete;
    Single2 &operator=(const Single2 &) = delete;
    static Single2 *GetInst()
    {
        std::lock_guard<std::mutex> lock(_mtx);
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
        return _instance;
    }
};
std::shared_ptr<singleOnceFlag> singleOnceFlag::_instance = nullptr;

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