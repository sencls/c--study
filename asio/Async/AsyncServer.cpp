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
#include "../../third_party/json/json.h"
#include "../../third_party/json/reader.h"
#include "Node.h"
#include "Const.h"
using boost::asio::ip::tcp;

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

class CSession : public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context &io_context, CServer *server) : _socket(io_context), _server(server), _b_close(false), _b_head_parse(false)
    {
        boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
        _uuid = boost::uuids::to_string(a_uuid);
        _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
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
        memset(_data, 0, MAX_LENGTH);
        _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH), std::bind(&HandleRead, this,
                                                                                  std::placeholders::_1, std::placeholders::_2, SharedSelf()));
    }

    void Send(std::string msg, short msgid)
    {
        std::lock_guard<std::mutex> lock(_send_lock);
        int send_que_size = _send_que.size();
        if (send_que_size > MAX_SENDQUE)
        {
            std::cout << "session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
            return;
        }

        _send_que.push(std::make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
        if (send_que_size > 0)
        {
            return;
        }
        auto &msgnode = _send_que.front();
        boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
                                 std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
    }
    void Send(char *msg, int max_length, short msgid)
    {
        std::lock_guard<std::mutex> lock(_send_lock);
        int send_que_size = _send_que.size();
        if (send_que_size > MAX_SENDQUE)
        {
            std::cout << "session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << std::endl;
            return;
        }

        _send_que.push(std::make_shared<SendNode>(msg, max_length, msgid));
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

    // void PrintRecvData(char *data, int length);
    // void HandleReadHand(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self);
    // void HandleReadMsg(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self);

    void HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self);
    void HandleRead(const boost::system::error_code &error, size_t bytes_transferred, std::shared_ptr<CSession> shared_self);
    tcp::socket _socket;
    std::string _uuid;
    char _data[MAX_LENGTH];
    CServer *_server;
    bool _b_close;
    std::queue<std::shared_ptr<SendNode>> _send_que;
    std::mutex _send_lock;
    // 收到的消息结构
    std::shared_ptr<RecvNode> _recv_msg_node;
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
    void ClearSession(std::string uuid)
    {
        _session.erase(uuid);
    }

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
    try
    {
        if (!error)
        {
            // 已经移动的字符数
            int copy_len = 0;
            while (bytes_transferred > 0)
            {
                if (!_b_head_parse)
                {
                    // 收到的数据不足头部大小
                    if (bytes_transferred + _recv_head_node->_cur_len < HEAD_TOTAL_LEN)
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
                    int head_remain = HEAD_TOTAL_LEN - _recv_head_node->_cur_len;
                    memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data + copy_len, head_remain);
                    // 更新已处理的data长度和剩余未处理的长度
                    copy_len += head_remain;
                    bytes_transferred -= head_remain;
                    // 获取头部MSGID数据
                    short msg_id = 0;
                    memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
                    // 网络字节序转化为本地字节序
                    msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
                    std::cout << "msg_id is " << msg_id << std::endl;
                    // id非法
                    if (msg_id > MAX_LENGTH)
                    {
                        std::cout << "invalid msg_id is " << msg_id << std::endl;
                        _server->ClearSession(_uuid);
                        return;
                    }
                    short msg_len = 0;
                    memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
                    // 网络字节序转化为本地字节序
                    msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
                    std::cout << "msg_len is " << msg_len << std::endl;
                    // id非法
                    if (msg_len > MAX_LENGTH)
                    {
                        std::cout << "invalid data length is " << msg_len << std::endl;
                        _server->ClearSession(_uuid);
                        return;
                    }

                    _recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);

                    // 消息的长度小于头部规定的长度，说明数据未收全，则先将部分消息放到接收节点里
                    if (bytes_transferred < msg_len)
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

                    memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, msg_len);
                    _recv_msg_node->_cur_len += msg_len;
                    copy_len += msg_len;
                    bytes_transferred -= msg_len;
                    _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
                    // cout << "receive data is " << _recv_msg_node->_data << endl;
                    // 此处可以调用Send发送测试
                    Json::Reader reader;
                    Json::Value root;
                    reader.parse(std::string(_recv_msg_node->_data, _recv_msg_node->_total_len), root);
                    std::cout << "recevie msg id  is " << root["id"].asInt() << " msg data is "
                              << root["data"].asString() << std::endl;
                    root["data"] = "server has received msg, msg data is " + root["data"].asString();
                    std::string return_str = root.toStyledString();
                    Send(return_str, root["id"].asInt());
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
                // cout << "receive data is " << _recv_msg_node->_data << endl;
                // 此处可以调用Send发送测试
                Json::Reader reader;
                Json::Value root;
                reader.parse(std::string(_recv_msg_node->_data, _recv_msg_node->_total_len), root);
                std::cout << "recevie msg id  is " << root["id"].asInt() << " msg data is "
                          << root["data"].asString() << std::endl;
                root["data"] = "server has received msg, msg data is " + root["data"].asString();
                std::string return_str = root.toStyledString();
                Send(return_str, root["id"].asInt());
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
    catch (std::exception &e)
    {
        std::cout << "Exception code is " << e.what() << std::endl;
    }
}
void CSession::HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self)
{
    // 增加异常处理
    try
    {
        if (!error)
        {
            std::lock_guard<std::mutex> lock(_send_lock);
            // cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
            _send_que.pop();
            if (!_send_que.empty())
            {
                auto &msgnode = _send_que.front();
                boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
                                         std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
            }
        }
        else
        {
            std::cout << "handle write failed, error is " << error.what() << std::endl;
            Close();
            _server->ClearSession(_uuid);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception code : " << e.what() << std::endl;
    }
}
// void CSession::HandleReadHand(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self)
// {
//     if (!ec)
//     {
//         if (bytes_transferred < HEAD_LENGTH)
//         {
//             std::cout << "read head length error";
//             Close();
//             _server->ClearSession(_uuid);
//             return;
//         }

//         // 头部解析
//         short data_len = 0;
//         memcpy(&data_len, _recv_head_node->_data, HEAD_TOTAL_LEN);
//         data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);
//         std::cout << "data len is " << data_len << std::endl;

//         if (data_len > MAX_LENGTH)
//         {
//             std::cout << "invalid data length is " << data_len << std::endl;
//             _server->ClearSession(_uuid);
//             return;
//         }

//         _recv_msg_node = std::make_shared<RecvNode>(data_len);
//         boost::asio::async_read(_socket, boost::asio::buffer(_recv_msg_node->_data, _recv_msg_node->_total_len),
//                                 [this, shared_self](const boost::system::error_code &ec, size_t bytes_transferred)
//                                 { HandleReadMsg(ec, bytes_transferred, shared_self); });
//     }
//     else
//     {
//         std::cout << "handle read head failed,error is " << ec.what() << std::endl;
//         Close();
//         _server->ClearSession(_uuid);
//     }
// }
// void CSession::HandleReadMsg(const boost::system::error_code &ec, size_t bytes_transferred, std::shared_ptr<CSession> shared_self)
// {
//     if (!ec)
//     {
//         PrintRecvData(_data, bytes_transferred);
//         std::chrono::milliseconds dura(2000);
//         std::this_thread::sleep_for(dura);
//         _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
//         std::cout << "receive data is" << _recv_msg_node->_data << std::endl;
//         Send(_recv_msg_node->_data, _recv_msg_node->_total_len);

//         _recv_head_node->Clear();
//         boost::asio::async_read(_socket, boost::asio::buffer(_recv_head_node->_data, HEAD_LENGTH),
//                                 [this, shared_self](const boost::system::error_code &ec, size_t size)
//                                 { HandleReadHand(ec, size, shared_self); });
//     }
//     else
//     {
//         std::cout << "handle read msg failed,error is " << ec.what() << std::endl;
//         Close();
//         _server->ClearSession(_uuid);
//     }
// }
// void CSession::PrintRecvData(char *data, int length)
// {
//     std::stringstream ss;
//     std::string result = "0x";
//     for (int i = 0; i < length; i++)
//     {
//         std::string hexstr;
//         ss << std::hex << std::setw(2) << std::setfill('0') << int(data[i]) << std::endl;
//         ss >> hexstr;
//         result += hexstr;
//     }
//     std::cout << "receive raw data is: " << result << std::endl;
// }
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