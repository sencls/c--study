#include "ConnectionMgr.h"
#include "Connection.h"

ConnectionMgr::ConnectionMgr() = default;

void ConnectionMgr::AddConnection(std::shared_ptr<Connection> conptr)
{
    _map_cons[conptr->GetUid()] = conptr;
}

void ConnectionMgr::RmvConnection(std::string id)
{
    _map_cons.erase(id);
}
