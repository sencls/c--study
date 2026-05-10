#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <iostream>
class AsioIOServicePool
{
public:
    using IOService = boost::asio::io_context;
    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using WorkPtr = std::unique_ptr<WorkGuard>;
    ~AsioIOServicePool() { std::cout << "AsioIOServicePool destruct\n"; }
    AsioIOServicePool(const AsioIOServicePool &) = delete;
    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;
    boost::asio::io_context &GetIOService()
    {
        auto &service = _ioService[_nextIOService++];
        if (_nextIOService == _ioService.size())
        {
            _nextIOService = 0;
        }
        return service;
    }
    void Stop()
    {
        for (auto &work : _works)
        {
            work.reset();
        }
        for (auto &t : _threads)
        {
            t.join();
        }
    }
    static AsioIOServicePool &GetInstance()
    {
        static AsioIOServicePool instance(1);
        return instance;
    }

private:
    AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency()) : _ioService(size), _nextIOService(0)
    {
        for (auto &io_service : _ioService)
        {
            _works.emplace_back(std::make_unique<WorkGuard>(boost::asio::make_work_guard(io_service)));
        }

        for (auto &ioc : _ioService)
        {
            _threads.emplace_back([this, &ioc]()
                                  { ioc.run(); });
        }
    }
    std::vector<IOService> _ioService;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t _nextIOService;
};