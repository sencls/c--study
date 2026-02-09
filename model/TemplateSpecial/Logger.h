#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
template <typename T, typename Enable = void>
class Logger
{
private:
public:
    static void log(const T &value)
    {
        std::cout << "General logger: " << value << std::endl;
    }
};
template <typename T>
class Logger<T, typename std::enable_if<std::is_pointer_v<T>>::type>
{
public:
    static void log(const T &value)
    {
        if (value)
        {
            std::cout << " Point logger: " << *value << std::endl;
        }
        else
            std::cout << "Point logger: nullptr\n";
    }
};
template <>
class Logger<std::string>
{
public:
    static void log(const std::string &value)
    {
        std::cout << " String logger: " << value << std::endl;
    }
};
template <typename T>
void logger(const T &value)
{
    Logger<T>::log(value);
}

template <typename... Args>
void loggerAll(const Args &...a)
{
    (logger(a), ...);
}
