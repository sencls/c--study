#pragma once

#include "singleton.h"
#include <boost/asio.hpp>
#include <vector>
#include <memory>

class IOServicePool : public Singleton<IOServicePool>
{
    friend Singleton<IOServicePool>;

public:
    using IOService = boost::asio::io_context;
    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using WorkPtr = std::unique_ptr<WorkGuard>;
    IOServicePool(const IOServicePool &) = delete;
    ~IOServicePool();
    IOServicePool &operator=(const IOServicePool &) = delete;
    boost::asio::io_context &GetIOService();
    void Stop();

private:
    IOServicePool(std::size_t size = std::thread::hardware_concurrency());
    std::vector<IOService> _ioService;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t _nextIOService;
};