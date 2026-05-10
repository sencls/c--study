#include <iostream>
#include <csignal>
#include <thread>
#include <mutex>
#include "CServer.h"
#include "AsioServicePool.h"

int main()
{

    try
    {
        auto &pool = AsioIOServicePool::GetInstance();
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, &pool](auto, auto)
                           {ioc.stop();
        pool.Stop(); });

        CServer s(ioc, 10086);
        ioc.run();
    }
    catch (const exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}