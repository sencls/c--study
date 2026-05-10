#pragma once

#include "singleton.h"
#include <boost/asio.hpp>
#include <memory>

class ThreadPool : public Singleton<ThreadPool>
{
public:
    friend class Singleton<ThreadPool>;

public:
    ~ThreadPool();
    ThreadPool();
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    boost::asio::io_context &GetIOService();
    void Stop();

private:
    ThreadPool(int threadNum = std::thread::hardware_concurrency());
    boost::asio::io_context _service;
    std::optional<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> _work;
    std::vector<std::thread> _threads;
};