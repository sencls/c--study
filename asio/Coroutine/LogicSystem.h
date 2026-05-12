#pragma once
#include <queue>
#include <thread>
#include "CSession.h"
#include <map>
#include <functional>
#include "Const.h"
#include "../../third_party/json/json.h"
#include "../../third_party/json/value.h"
#include "../../third_party/json/reader.h"

typedef function<void(std::shared_ptr<CSession>, const short &msg_id, const string &msg_data)> FunCallBack;

class LogicSystem
{
public:
    LogicSystem() : _b_stop(false)
    {
        RegisterCallBacks();
        _worker_thread = std::thread(&LogicSystem::DealMsg, this);
    }
    ~LogicSystem()
    {
        _b_stop = true;
        _consume.notify_one();
        _worker_thread.join();
    }
    void PostMsgToQue(std::shared_ptr<LogicNode> msg)
    {
        std::unique_lock<std::mutex> unique_lk(_mutex);
        _msg_que.push(msg);
        if (_msg_que.size() == 1)
        {
            _consume.notify_one();
        }
    }
    static LogicSystem &Getinstance()
    {
        static LogicSystem instance;
        return instance;
    }
    LogicSystem(const LogicSystem &) = delete;
    LogicSystem &operator=(const LogicSystem &) = delete;

private:
    void DealMsg()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> unique_lk(_mutex);
            while (_msg_que.empty() && !_b_stop)
            {
                _consume.wait(unique_lk);
            }

            if (_b_stop)
            {
                while (!_msg_que.empty())
                {
                    auto msg_node = _msg_que.front();
                    std::cout << "recv msg id is " << msg_node->_recvnode->_msg_id << std::endl;
                    auto callback = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
                    if (callback == _fun_callbacks.end())
                    {
                        _msg_que.pop();
                        continue;
                    }

                    callback->second(msg_node->_session, msg_node->_recvnode->_msg_id, std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_total_len));

                    _msg_que.pop();
                }

                break;
            }

            auto msg_node = _msg_que.front();
            std::cout << "recv_msg id is " << msg_node->_recvnode->_msg_id << std::endl;
            auto callback = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
            if (callback == _fun_callbacks.end())
            {
                _msg_que.pop();
                continue;
            }

            callback->second(msg_node->_session, msg_node->_recvnode->_msg_id, std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_total_len));

            _msg_que.pop();
        }
    }
    void RegisterCallBacks()
    {
        _fun_callbacks[MSG_HELLO_WORD] = [this](std::shared_ptr<CSession> a, const short &msg_id, const string &msg_data)
        { HelloWorldCallBack(a, msg_id, msg_data); };
    }
    void HelloWorldCallBack(std::shared_ptr<CSession> session, const short &msg_id, const string &msg_data)
    {
        Json::Reader reader;
        Json::Value root;
        reader.parse(msg_data, root);
        std::cout << "receive msg id is " << root["id"].asInt() << " msg data is " << root["data"].asString() << std::endl;

        root["data"] = "server has received msg, msg data is " + root["data"].asString();

        std::string return_str = root.toStyledString();
        session->Send(return_str, root["id"].asInt());
    }

    std::thread _worker_thread;
    std::queue<std::shared_ptr<LogicNode>> _msg_que;
    std::mutex _mutex;
    std::condition_variable _consume;
    bool _b_stop;
    std::map<short, FunCallBack> _fun_callbacks;
};