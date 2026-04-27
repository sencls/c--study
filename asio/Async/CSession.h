#pragma once
#include <boost/asio.hpp>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include "Node.h"
#include "singleton.h"
#include "Const.h"

using boost::asio::ip::tcp;

class CServer;

class CSession : public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context &io_context, CServer *server);
    ~CSession();

    tcp::socket &GetSocket();
    std::string &GetUuid();
    void Start();
    void Send(std::string msg, short msgid);
    void Send(char *msg, int max_length, short msgid);
    void Close();
    std::shared_ptr<CSession> SharedSelf();

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

class LogicNode
{
    friend class LogicSystem;

public:
    LogicNode(std::shared_ptr<CSession>, std::shared_ptr<RecvNode>);

private:
    std::shared_ptr<CSession> _session;
    std::shared_ptr<RecvNode> _recvnode;
};
