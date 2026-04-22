#pragma once
#include <string>
#include "Const.h"
#include <iostream>
#include <boost/asio.hpp>
using namespace std;
using boost::asio::ip::tcp;
class MsgNode
{
public:
    MsgNode(short max_len) : _total_len(max_len), _cur_len(0)
    {
        _data = new char[_total_len + 1]();
        _data[_total_len] = '\0';
    }

    ~MsgNode()
    {
        std::cout << "destruct MsgNode" << endl;
        delete[] _data;
    }

    void Clear()
    {
        ::memset(_data, 0, _total_len);
        _cur_len = 0;
    }

    short _cur_len;
    short _total_len;
    char *_data;
};

class RecvNode : public MsgNode
{
public:
    RecvNode(short max_len, short msg_id) : MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id) {}

private:
    short _msg_id;
};

class SendNode : public MsgNode
{
public:
    SendNode(const char *msg, short max_len, short msg_id) : MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id)
    {
        // 先发送id, 转为网络字节序
        short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
        memcpy(_data, &msg_id_host, HEAD_ID_LEN);
        // 转为网络字节序
        short max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
        memcpy(_data + HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);
        memcpy(_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
    }

private:
    short _msg_id;
};