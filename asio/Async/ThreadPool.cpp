#include "ThreadPool.h"
#include <optional>

ThreadPool::ThreadPool(int threadNum) : _work(boost::asio::make_work_guard(_service))
{
    for (int i = 0; i < threadNum; ++i)
    {
        _threads.emplace_back([this]()
                              { _service.run(); });
    }
}

boost::asio::io_context &ThreadPool::GetIOService()
{
    return _service;
}

void ThreadPool::Stop()
{
    _work.reset();
    for (auto &t : _threads)
    {
        t.join();
    }
}