#pragma once
#include "ConnectionMgr.h"
#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include "Connection.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class WebSocketServer
{
public:
    WebSocketServer(const WebSocketServer &) = delete;
    WebSocketServer &operator=(const WebSocketServer &) = delete;
    WebSocketServer(net::io_context &ioc, unsigned short port) : _ioc(ioc), _acceptor(ioc, net::ip::tcp::endpoint(net::ip::tcp::v4(), port))
    {
        std::cout << "Server start on port: " << port << std::endl;
    }
    void StartAccept()
    {
        auto con_ptr = std::make_shared<Connection>(_ioc);
        _acceptor.async_accept(con_ptr->GetSocket(), [this, con_ptr](boost::system::error_code err)
                               {
            try
            {
                if (!err)
                {
                    con_ptr->AsyncAccept();
                }
                else
                {
                    std::cout << "acceptor async_accept failed,er is " << err.what() << std::endl;
                }
                StartAccept();
            }
            catch (const std::exception &e)
            {
                std::cout << "async_accept error is " << e.what() << std::endl;
            } });
    }

private:
    net::ip::tcp::acceptor _acceptor;
    net::io_context &_ioc;
};