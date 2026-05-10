#include <boost/asio/co_spawn.hpp>   //启动携程的api
#include <boost/asio/detached.hpp>   //分离器
#include <boost/asio/io_context.hpp> //执行器
#include <boost/asio/ip/tcp.hpp>     //域名解析
#include <iostream>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>

using boost::asio::awaitable; // 可以异步等待
using boost::asio::co_spawn;  // 启动携程的关键字
using boost::asio::detached;  // 独立启动
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

awaitable<void> echo(tcp::socket socket)
{
    try
    {
        char data[1024];
        for (;;)
        {
            std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
            co_await async_write(socket, boost::asio::buffer(data, n), use_awaitable);
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "echo exception is " << e.what() << '\n';
    }
}

awaitable<void> listener()
{
    auto executor = co_await this_coro::executor; // 返回携程的调度器，co_await异步查询
    tcp::acceptor acceptor(executor, {tcp::v4(), 10086});
    for (;;)
    {
        tcp::socket sock = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor, echo(std::move(sock)), detached);
    }
}
int main()
{
    try
    {
        boost::asio::io_context ioc(1); // 通过括号指定构造，对象的并发优先级；
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto)
                           { ioc.stop(); });
        co_spawn(ioc, listener(), detached);

        //  // co_spawn(ioc,listener,detached)
        //   像这样写两个 co_spawn 时，它们是 并发执行 的，原因如下：

        //   1. co_spawn 本身是非阻塞的 —— 它只是把协程"提交"到 io_context
        //   的任务队列中，然后立即返回，不会等待协程执行完毕。
        //   2. 执行顺序：
        //     - 第一个 co_spawn → 将 coroutine_a 注册到 ioc，立即返回
        //     - 第二个 co_spawn → 将 coroutine_b 注册到 ioc，立即返回
        //     - 当 ioc.run() 被调用时，两个协程在同一个事件循环中交替调度执行
        //   3. 调度模型：协程在遇到 co_await 挂起点时让出控制权，io_context
        //   会切换到另一个就绪的协程继续执行。这就是协程级并发的精髓。

        ioc.run();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << '\n';
    }

    return 0;
}