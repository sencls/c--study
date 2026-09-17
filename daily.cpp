#include <iostream>
#include <thread>
#include <vector>

// int main()
// {
//     pans::Singleton::Instance().log("Hello Singleton.");
//     return 0;
//}

template <typename T>
class Singleton
{
public:
    static inline T &Instance()
    {
        static T instance;
        return instance;
    }
    Singleton(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton &operator=(const Singleton &) = delete;
    Singleton &operator=(Singleton &&) = delete;

private:
    Singleton() = delete;
};
class A
{
public:
    void log(const std::string &msg)
    {
        std::cout << "[LOG]: " << msg << std::endl;
    }
    A(const A &) = delete;
    A(A &&) = delete;
    A &operator=(const A &) = delete;
    A &operator=(A &&) = delete;

private:
    A() = default;
    ~A() = default;

    friend class Singleton<A>;
};

using SA = Singleton<A>;

int main()
{
    SA::Instance().log("hello Singleton2.");
    return 0;
}