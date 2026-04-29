#include <boost/asio.hpp>
#include <iostream>
#include "CServer.h"

#include <csignal>
#include <thread>
#include <mutex>
#include "IOServicePool.h"
using namespace std;
bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

void sig_handle(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    {
        std::unique_lock<std::mutex> lock_quit(mutex_quit);
        bstop = true;
        cond_quit.notify_one();
    }
}

int main()
{
    try
    {
        //     boost::asio::io_context io_context;
        //     std::thread net_work_thread([&io_context]()
        //                                 {
        //         CServer s(io_context, 10086);
        //     io_context.run(); });

        //     signal(SIGINT, sig_handle);
        //     signal(SIGTERM, sig_handle);

        //     while (!bstop)
        //     {
        //         std::unique_lock<std::mutex> lock_quit(mutex_quit);
        //         cond_quit.wait(lock_quit);
        //     }

        //     io_context.stop();
        //     net_work_thread.join();

        auto pool = IOServicePool::GetInstacne();
        boost::asio::io_context io_context;
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context](auto, auto)
                           { io_context.stop(); });

        CServer s(io_context, 10086);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
