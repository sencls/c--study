#pragma once

#include <memory>
#include <mutex>
#include <iostream>

using namespace std;

template <typename T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton<T> &) = delete;
    Singleton &operator=(const Singleton<T> &st) = delete;

    static shared_ptr<T> _instance;

public:
    ~Singleton()
    {
        cout << "this is singleton destruct" << endl;
    }
    static shared_ptr<T> GetInstacne()
    {
        static once_flag s_flag;

        std::call_once(s_flag, [&]()
                       { _instance = shared_ptr<T>(new T); });

        return _instance;
    }

    void PrintAddress()
    {
        cout << _instance->get() << endl;
    }
};
template <typename T>
shared_ptr<T> Singleton<T>::_instance = nullptr;