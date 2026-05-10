#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <memory>
#include "CServer.h"
#include "Msg.h"
#include <mutex>
#include <memory>
#include "Const.h"
#include <queue>

using boost::asio::detached;
using boost::asio::io_context;
using boost::asio::strand;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

class CServer;
class CSession : public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context &io_context, CServer *server) : _io_context(io_context), _server(server), _socket(io_context), _b_close(false)
    {
        boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
        _uuid = boost::uuids::to_string(a_uuid);
        _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
    }
    ~CSession()
    {
        try
        {
            std::cout << "~CSession destruct\n";
            Close();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    boost::asio::ip::tcp::socket &GetSocket()
    {
        return _socket;
    }
    std::string &GetUuid()
    {
        return _uuid;
    }
    void Close()
    {
        _socket.close();
        _b_close = true;
        _server->ClearSession(_uuid);
    }
    void Start()
    {
        auto shared_this = shared_from_this();
        // 开启协程
        boost::asio::co_spawn(_io_context, [=]() -> boost::asio::awaitable<void>
                              {
                     try
                     {
                        for(;!_b_close;)
                        {
                            _recv_head_node->Clear();
                            std::size_t n=co_await boost::asio::async_read(_socket,boost::asio::buffer(_recv_head_node->_data,HEAD_TOTAL_LEN),use_awaitable);

                            if(n==0)
                            {
                                std::cout <<"receive peer closed\n"; 
                                Close();
                                _server->ClearSession(_uuid);
                                co_return;
                            }

                            short msg_id=0;
                            memcpy(&msg_id,_recv_head_node->_data,HEAD_ID_LEN);

                            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
                            std::cout <<"msg_id is "<<msg_id <<std::endl;
                            if(msg_id>MAX_LENGTH)
                            {
                                std::cout <<"invalid msg_id is "<<msg_id <<std::endl;
                                Close();
                                _server->ClearSession(_uuid);
                                co_return;
                            }

                            short msg_len=0;
                            memcpy(&msg_len,_recv_head_node->_data+HEAD_ID_LEN,HEAD_DATA_LEN);
                            msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
                            std::cout <<"msg len is " <<msg_len <<std::endl;
                            if(msg_len >MAX_LENGTH)
                            {
                                std::cout <<"invalid msg len is "<<msg_len <<std::endl;
                                Close();
                                _server->ClearSession(_uuid);
                                co_return;
                            }

                            _recv_head_node =std::make_shared<RecvNode>(msg_len,msg_id);

                            n=co_await boost::asio::async_read(_socket,boost::asio::buffer(_recv_msg_node->_data,_recv_msg_node->_total_len),use_awaitable);

                            if(n==0)
                            {
                                std::cout <<"receive peer closed\n";
                                Close();
                                _server->ClearSession(_uuid);
                                co_return;
                            }

                            _recv_msg_node->_data[_recv_msg_node->_total_len]='\0';
                            std::cout <<"receive data is "<<_recv_msg_node->_data <<std::endl;


                        }
                     }
                     catch (const std::exception &e)
                     {
                         std::cerr << e.what() << '\n';
                         Close();
                     } }, detached);
    }
    void Send(const char *msg, short max_length, short msgid)
    {
        std::unique_lock<std::mutex> lock(_send_lock);
        int send_que_size = _send_que.size();
        if (send_que_size > MAX_SENDQUE)
        {
            std::cout << "session: " << _uuid << " send que fulled ,size is " << MAX_SENDQUE << std::endl;
            return;
        }

        _send_que.push(std::make_shared<SendNode>(msg, max_length, msgid));

        auto msgnode = _send_que.front();
        lock.unlock();
        boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len), [this](auto a, auto b)
                                 { HandleWrite(a, shared_from_this()); });
    }
    void Send(std::string msg, short msgid)
    {
        Send(msg.c_str(), msg.length(), msgid);
    }
    void HandleWrite(const boost::system::error_code &ec, std::shared_ptr<CSession> shared_self)
    {
        try
        {
            if (!ec)
            {
                std::unique_lock<std::mutex> lock(_send_lock);
                _send_que.pop();
                if (!_send_que.empty())
                {
                    auto msgnode = _send_que.front();
                    lock.unlock();
                    boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len), [this](auto a, auto b)
                                             { HandleWrite(a, shared_from_this()); });
                }
            }
            else
            {
                std::cout << "handle write failed,error is " << ec.what() << std::endl;
                Close();
                _server->ClearSession(_uuid);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            Close();
            _server->ClearSession(_uuid);
        }
    }

private:
    boost::asio::io_context &_io_context;
    CServer *_server;
    boost::asio::ip::tcp::socket _socket;
    std::string _uuid;
    bool _b_close;
    std::mutex _send_lock;
    std::queue<std::shared_ptr<SendNode>> _send_que;
    std::shared_ptr<RecvNode> _recv_msg_node;
    std::shared_ptr<MsgNode> _recv_head_node;
};

class LogicNode
{
public:
    LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode) : _session(session), _recvnode(recvnode)
    {
    }
    std::shared_ptr<CSession> _session;
    std::shared_ptr<RecvNode> _recvnode;
};