#pragma once

#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <queue>
#include <mutex>
#include "ConnectionMgr.h"

namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;

class Connection : public std::enable_shared_from_this<Connection>
{

public:
    Connection(net::io_context &ioc) : _ioc(ioc), _ws_ptr(std::make_unique<stream<tcp_stream>>(make_strand(ioc)))
    {
        boost::uuids::random_generator generator;
        boost::uuids::uuid uuid = generator();
        _uuid = boost::uuids::to_string(uuid);
    }
    std::string GetUid()
    {
        return _uuid;
    }
    net::ip::tcp::socket &GetSocket()
    {
        return boost::beast::get_lowest_layer(*_ws_ptr).socket();
    }
    void AsyncAccept()
    {
        auto self = shared_from_this();
        _ws_ptr->async_accept([self](boost::system::error_code err)
                              {
                                  try
                                  {
                                      if (!err)
                                      {
                                        ConnectionMgr::GetInstance().AddConnection(self);
                                        self->Start();
                                      }
                                      else
                                      {
                                          std::cout << "websocket accept failed,err is " << err.what() << std::endl;
                                      }
                                  }
                                  catch (const std::exception &e)
                                  {
                                      std::cout << "websocket async accept exception is " << e.what() << '\n';
                                  } });
    }
    void Start()
    {
        auto self = shared_from_this();
        _ws_ptr->async_read(_recv_buffer, [self](error_code err, std::size_t buffer_bytes)
                            {
                                try
                                {
                                    if(err)
                                    {
                                       std::cout <<"websocket asynce read error is " <<err.what() <<std::endl;
                                       ConnectionMgr::GetInstance().RmvConnection(self->GetUid());
                                       return;
                                    }
                                    self->_ws_ptr->text(self->_ws_ptr->got_text());
                                    std::string recv_data =boost::beast::buffers_to_string(self->_recv_buffer.data());
                                    self->_recv_buffer.consume(self->_recv_buffer.size());
                                    std::cout <<"websocket receive msg is "  <<recv_data<<std::endl;

                                    self->AsyncSend(std::move(recv_data));
                                }
                                catch (const std::exception &e)
                                {
                                    std::cerr << "exception is " << e.what() << '\n';
                                    ConnectionMgr::GetInstance().RmvConnection(self->GetUid());
                                    return;
                                } });
    }

    void AsyncSend(std::string msg)
    {
        {
            std::lock_guard<std::mutex> lck_gurad(_send_mtx);
            int que_len = _send_que.size();
            _send_que.push(msg);
            if (que_len > 0)
            {
                return;
            }
        }

        SendCallBack(std::move(msg));
    }

    void SendCallBack(std::string msg)
    {
        auto self = shared_from_this();
        _ws_ptr->async_write(boost::asio::buffer(msg.c_str(), msg.length()),
                             [self](error_code err, std::size_t nsize)
                             {
                                 try
                                 {
                                     if (err)
                                     {
                                         std::cout << "async send err is " << err.what() << std::endl;
                                         ConnectionMgr::GetInstance().RmvConnection(self->_uuid);
                                         return;
                                     }

                                     std::string send_msg;
                                     {
                                         std::lock_guard<std::mutex> lck_gurad(self->_send_mtx);
                                         self->_send_que.pop();
                                         if (self->_send_que.empty())
                                         {
                                             return;
                                         }

                                         send_msg = self->_send_que.front();
                                     }

                                     self->SendCallBack(std::move(send_msg));
                                 }
                                 catch (std::exception &exp)
                                 {
                                     std::cout << "async send exception is " << exp.what() << std::endl;
                                     ConnectionMgr::GetInstance().RmvConnection(self->_uuid);
                                 }
                             });
    }

private:
    std::unique_ptr<stream<tcp_stream>>
        _ws_ptr;
    std::string _uuid;
    net::io_context &_ioc;
    flat_buffer _recv_buffer;
    std::queue<std::string> _send_que;
    std::mutex _send_mtx;
};