#pragma once

#include "Singleton.h"
#include <queue>
#include <thread>
#include <map>
#include <functional>
#include "CSession.h"
#include "../../third_party/json/json.h"
#include "../../third_party/json/value.h"
#include "../../third_party/json/reader.h"

typedef function<void(shared_ptr<CSession>, const short &msg_id, const string &msg_data)> FunCallBack;
class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();
    void PostMsgToQue(std::shared_ptr<LogicNode> msg);

private:
    LogicSystem();
    void RegisterCallBacks();
    void HelloWordCallBack(std::shared_ptr<CSession>, const short &msg_id, const string &msg_data);
    void DealMsg();

    std::queue<shared_ptr<LogicNode>> _msg_que;
    std::mutex _mutex;
    std::condition_variable _consume;
    std::thread _worker_thread;
    bool _b_stop;
    std::map<short, FunCallBack> _fun_callback;
};
