#include "IOServicePool.h"

IOServicePool::IOServicePool(std::size_t size) : _ioService(size), _nextIOService(0)
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

IOServicePool::~IOServicePool()
{
    std::cout << "IOServicePool destruct\n";
}

boost::asio::io_context &IOServicePool::GetIOService()
{
    auto &service = _ioService[_nextIOService++];
    if (_nextIOService == _ioService.size())
        _nextIOService = 0;

    return service;
}

void IOServicePool::Stop()
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