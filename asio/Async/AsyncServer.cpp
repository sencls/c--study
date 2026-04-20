#include <boost/asio.hpp>
#include <string>
#include <boost/uuid/generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <iomanip>
#include <memory>
#include <map>
#include <queue>
#include <sstream>
using boost::asio::ip::tcp;
#define MAX_LENGTH 1024 * 2
#define HEAD_LENGTH 2
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 1000
// // 木铎网络库muduo class Session
// // {
// // public:
// //     Session(boost::asio::io_context &ioc) : _socket(ioc) {}
// //     tcp::socket &Socket()
// //     {
// //         return _socket;
// //     }
// //     void Start()
// //     {
// //         memset(_data, 0, max_length);
// //         _socket.async_read_some(boost::asio::buffer(_data, max_length),
// //                                 [this](const boost::system::error_code &ec, std::size_t bytes_transferrecd)
// //                                 {
// //                                     handle_read(ec, bytes_transferrecd);
// //                                 });
// //     }

// // private:
// //     void handle_read(const boost::system::error_code &ec, std::size_t bytes_transferrecd)
// //     {
// //         if (!ec)
// //         {
// //             std::cout << "server receive data is: " << _data << std::endl;
// //             boost::asio::async_write(_socket, boost::asio::buffer(_data, bytes_transferrecd),
// //                                      [this](const boost::system::error_code &ec, std::size_t /*bytes_sent*/)
// //                                      {
// //                                          handle_write(ec);
// //                                      });
// //         }
// //         else
// //         {
// //             std::cout << "read error" << std::endl;
// //             delete this;
// //         }
// //     }
// //     void handle_write(const boost::system::error_code &ec)
// //     {
// //         if (!ec)
// //         {
// //             memset(_data, 0, max_length);
// //             _socket.async_read_some(boost::asio::buffer(_data, max_length),
// //                                     [this](const boost::system::error_code &ec, std::size_t bytes_transferrecd)
// //                                     {
// //                                         handle_read(ec, bytes_transferrecd);
// //                                     });
// //         }
// //         else
// //         {
// //             std::cout << "write error" << std::endl;
// //             delete this;
// //         }
// //     }
// //     tcp::socket _socket;
// //     enum
// //     {
// //         max_length = 1024
// //     };
// //     char _data[max_length];
// // };
// // class Server
// // {
// // private:
// //     void start_accept()
// //     {
// //         Session *new_session = new Session(_ioc);
// //         _acceptor.async_accept(new_session->Socket(), [this, &new_session](const boost::system::error_code &ec)
// //                                { handle_accept(new_session, ec); });
// //     }
// //     void handle_accept(Session *new_session, const boost::system::error_code &ec)
// //     {
// //         if (!ec)
// //         {
// //             new_session->Start();
// //         }
// //         else
// //         {
// //             std::cout << "failed" << std::endl;
// //             delete new_session;
// //         }
// //         start_accept();
// //     }
// //     boost::asio::io_context &_ioc;
// //     tcp::acceptor _acceptor;
// //     伪闭包实现 std::map<std::string, std::shared_ptr<Session>> _session;

// public:
//     Server(boost::asio::io_context &ioc, short port) : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
//     {
//         std::cout << "Server start success,on port : " << port << std::endl;
//         start_accept();
//     }
// };
class CServer;

class MsgNode
{
    friend class CSession;

public:
    MsgNode(char *msg, short max_len) : _total_len(max_len + HEAD_LENGTH), _cur_len(0)
    {
        _data = new char[_total_len + 1]();
        short max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
        memcpy(_data, &max_len_host, HEAD_LENGTH);
        memcpy(_data + HEAD_LENGTH, msg, max_len), _data[_total_len] = '\0';
    }

    MsgNode(short max_len) : _total_len(max_len), _cur_len(0)
    {
        _data = new char[_total_len + 1]();
    }
    ~MsgNode()
    {
        delete[] _data;
    }

    void Clear()
    {
        memset(_data, 0, _total_len);
        _cur_len = 0;
    }

private:
    short _cur_len;
    short _total_len;
    char *_data;
};

class CSession : public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context &io_context, CServer *server) : _socket(io_context), _server(server), _b_close(false), _b_head_parse(false)
    {
        boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
        _uuid = boost::uuids::to_string(a_uuid);
        _recv_head_node = std::make_shared<MsgNode>(HEAD_LENGTH);
    }
    ~CSession()
    {
        std::cout << "~CSession destruct" << std::endl;
    }

    tcp::socket &GetSocket()
    {
        return _socket;
    }

    std::string &GetUuid()
    {
        return _uuid;
    }

    void Start()
    {
        _recv_head_node->Clear();
        boost::asio::async_read(_socket, boost::asio::buffer(_recv_head_node->_data, HEAD_LENGTH),
                                [this](const boost::system::error_code &ec, size_t size)
                                { HandleReadHand(ec, size, SharedSelf()); });
    }

    void Send(char *msg, int max_length)
    {
        std::lock_guard<std::mutex> lock(_send_lock);
        int send_que_size = _send_que.size();
        if (send_que_size > MAX_SENDQUE)
        {
            std::cout << "session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
            return;
        }

        _send_que.push(std::make_shared<MsgNode>(msg, max_length));
        if (send_que_size > 0)
        {
            return;
        }
        auto &msgnode = _send_que.front();
        boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
                                 std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
    }

    void Close()
    {
        _socket.close();
        _b_close = true;
    }

    std::shared_ptr<CSession> SharedSelf()
    {
        return shared_from_this();
    }

    void PrintRecvData(char *data, int length);
    void HandleReadHand(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self);
    void HandleReadMsg(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self);

    void HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self);
    void HandleRead(const boost::system::error_code &error, size_t bytes_transferred, std::shared_ptr<CSession> shared_self);
    tcp::socket _socket;
    std::string _uuid;
    char _data[MAX_LENGTH];
    CServer *_server;
    bool _b_close;
    std::queue<std::shared_ptr<MsgNode>> _send_que;
    std::mutex _send_lock;
    // 收到的消息结构
    std::shared_ptr<MsgNode> _recv_msg_node;
    bool _b_head_parse;
    // 收到的头部结构
    std::shared_ptr<MsgNode> _recv_head_node;
};

class CServer
{
public:
    CServer(boost::asio::io_context &ioc, short port) : _ioc(ioc), _port(port), _acceptor(_ioc, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server start success,listen on port " << _port << std::endl;
        StartAccept();
    }
    void ClearSession(std::string) {}

private:
    void HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code &ec)
    {
        if (!ec)
        {
            new_session->Start();
            _session.insert(make_pair(new_session->GetUuid(), new_session));
        }
        else
        {
            std::cout << "session accept failed,error is " << ec.what() << std::endl;
        }

        StartAccept();
    }
    void StartAccept()
    {
        std::shared_ptr<CSession> new_session = std::make_shared<CSession>(_ioc, this);
        _acceptor.async_accept(new_session->GetSocket(), [this, new_session](const boost::system::error_code &ec)
                               { HandleAccept(new_session, ec); });
    }
    boost::asio::io_context &_ioc;
    short _port;
    tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<CSession>> _session;
};
void CSession::HandleRead(const boost::system::error_code &error, size_t bytes_transferred, std::shared_ptr<CSession> shared_self)
{
    // 比较复杂，在于async_read_some函数是只要有数据的传递，就会返回数据，所以长度是不可控的；
    if (!error)
    {
        // 已经移动的字符数
        int copy_len = 0;
        while (bytes_transferred > 0)
        {
            if (!_b_head_parse)
            {
                // 收到的数据不足头部大小
                if (bytes_transferred + _recv_head_node->_cur_len < HEAD_LENGTH)
                {
                    memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data + copy_len, bytes_transferred);
                    _recv_head_node->_cur_len += bytes_transferred;
                    ::memset(_data, 0, MAX_LENGTH);
                    _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
                                            std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                    return;
                }
                // 收到的数据比头部多
                // 头部剩余未复制的长度
                int head_remain = HEAD_LENGTH - _recv_head_node->_cur_len;
                memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data + copy_len, head_remain);
                // 更新已处理的data长度和剩余未处理的长度
                copy_len += head_remain;
                bytes_transferred -= head_remain;
                // 获取头部数据
                short data_len = 0;
                memcpy(&data_len, _recv_head_node->_data, HEAD_LENGTH);
                // 网络字节序转化为本地字节序
                data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);
                std::cout << "data_len is " << data_len << std::endl;
                // 头部长度非法
                if (data_len > MAX_LENGTH)
                {
                    std::cout << "invalid data length is " << data_len << std::endl;
                    _server->ClearSession(_uuid);
                    return;
                }
                _recv_msg_node = std::make_shared<MsgNode>(data_len);

                // 消息的长度小于头部规定的长度，说明数据未收全，则先将部分消息放到接收节点里
                if (bytes_transferred < data_len)
                {
                    memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, bytes_transferred);
                    _recv_msg_node->_cur_len += bytes_transferred;
                    ::memset(_data, 0, MAX_LENGTH);
                    _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
                                            std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                    // 头部处理完成
                    _b_head_parse = true;
                    return;
                }

                memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, data_len);
                _recv_msg_node->_cur_len += data_len;
                copy_len += data_len;
                bytes_transferred -= data_len;
                _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
                std::cout << "receive data is " << _recv_msg_node->_data << std::endl;
                // 此处可以调用Send发送测试
                Send(_recv_msg_node->_data, _recv_msg_node->_total_len);
                // 继续轮询剩余未处理数据
                _b_head_parse = false;
                _recv_head_node->Clear();
                if (bytes_transferred <= 0)
                {
                    ::memset(_data, 0, MAX_LENGTH);
                    _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
                                            std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                    return;
                }
                continue;
            }

            // 已经处理完头部，处理上次未接受完的消息数据
            // 接收的数据仍不足剩余未处理的
            int remain_msg = _recv_msg_node->_total_len - _recv_msg_node->_cur_len;
            if (bytes_transferred < remain_msg)
            {
                memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, bytes_transferred);
                _recv_msg_node->_cur_len += bytes_transferred;
                ::memset(_data, 0, MAX_LENGTH);
                _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
                                        std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                return;
            }
            memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, remain_msg);
            _recv_msg_node->_cur_len += remain_msg;
            bytes_transferred -= remain_msg;
            copy_len += remain_msg;
            _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
            std::cout << "receive data is " << _recv_msg_node->_data << std::endl;
            // 此处可以调用Send发送测试
            Send(_recv_msg_node->_data, _recv_msg_node->_total_len);
            // 继续轮询剩余未处理数据
            _b_head_parse = false;
            _recv_head_node->Clear();
            if (bytes_transferred <= 0)
            {
                ::memset(_data, 0, MAX_LENGTH);
                _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
                                        std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
                return;
            }
            continue;
        }
    }
    else
    {
        std::cout << "handle read failed, error is " << error.what() << std::endl;
        Close();
        _server->ClearSession(_uuid);
    }
}
void CSession::HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self)
{
    if (!error)
    {
        std::lock_guard<std::mutex> lock(_send_lock);
        std::cout << "send data " << _send_que.front()->_data + HEAD_LENGTH << std::endl;
        _send_que.pop();
        if (!_send_que.empty())
        {
            auto &msgnode = _send_que.front();
            boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
                                     [this, shared_self](const boost::system::error_code &ec, std::size_t /*size */)
                                     { HandleWrite(ec, shared_self); });
        }
    }
    else
    {
        std::cout << "handle write failed, error is " << error.what() << std::endl;
        Close();
        _server->ClearSession(_uuid);
    }
}
void CSession::HandleReadHand(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self)
{
    if (!ec)
    {
        if (bytes_transferred < HEAD_LENGTH)
        {
            std::cout << "read head length error";
            Close();
            _server->ClearSession(_uuid);
            return;
        }

        // 头部解析
        short data_len = 0;
        memcpy(&data_len, _recv_head_node->_data, HEAD_LENGTH);
        data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);
        std::cout << "data len is " << data_len << std::endl;

        if (data_len > MAX_LENGTH)
        {
            std::cout << "invalid data length is " << data_len << std::endl;
            _server->ClearSession(_uuid);
            return;
        }

        _recv_msg_node = std::make_shared<MsgNode>(data_len);
        boost::asio::async_read(_socket, boost::asio::buffer(_recv_msg_node->_data, _recv_msg_node->_total_len),
                                [this, shared_self](const boost::system::error_code &ec, size_t bytes_transferred)
                                { HandleReadMsg(ec, bytes_transferred, shared_self); });
    }
    else
    {
        std::cout << "handle read head failed,error is " << ec.what() << std::endl;
        Close();
        _server->ClearSession(_uuid);
    }
}
void CSession::HandleReadMsg(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self)
{
    if (!ec)
    {
        PrintRecvData(_data, bytes_transferred);
        std::chrono::milliseconds dura(2000);
        std::this_thread::sleep_for(dura);
        _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
        std::cout << "receive data is" << _recv_msg_node->_data << std::endl;
        Send(_recv_msg_node->_data, _recv_msg_node->_total_len);

        _recv_head_node->Clear();
        boost::asio::async_read(_socket, boost::asio::buffer(_recv_head_node->_data, HEAD_LENGTH),
                                [this, shared_self](const boost::system::error_code &ec, size_t size)
                                { HandleReadHand(ec, size, shared_self); });
    }
    else
    {
        std::cout << "handle read msg failed,error is " << ec.what() << std::endl;
        Close();
        _server->ClearSession(_uuid);
    }
}
void CSession::PrintRecvData(char *data, int length)
{
    std::stringstream ss;
    std::string result = "0x";
    for (int i = 0; i < length; i++)
    {
        std::string hexstr;
        ss << std::hex << std::setw(2) << std::setfill('0') << int(data[i]) << std::endl;
        ss >> hexstr;
        result += hexstr;
    }
    std::cout << "receive raw data is: " << result << std::endl;
}
int main()
{
    try
    {
        boost::asio::io_context io_context;
        CServer s(io_context, 10086);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}