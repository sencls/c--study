#pragma once
#include <boost/unordered_map.hpp>
#include <memory>
#include <string>

class Connection;

class ConnectionMgr
{
public:
    static ConnectionMgr &GetInstance()
    {
        static ConnectionMgr mgr;
        return mgr;
    }
    void AddConnection(std::shared_ptr<Connection> conptr);
    void RmvConnection(std::string id);

private:
    ConnectionMgr(const ConnectionMgr &) = delete;
    ConnectionMgr &operator=(const ConnectionMgr) = delete;
    ConnectionMgr();
    boost::unordered_map<std::string, std::shared_ptr<Connection>> _map_cons;
};
