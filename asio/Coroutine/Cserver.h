#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <map>
#include "CSession.h"
#include "AsioServicePool.h"

using boost::asio::ip::tcp;
using namespace std;

class CServer
{
public:
    CServer(boost::asio::io_context &io_context, short port) : _io_context(io_context), _port(port), _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    {
        StartAccept();
    }
    ~CServer() {}
    void ClearSession(string uuid)
    {
        lock_guard<mutex> lock(_mutex);
        _session.erase(uuid);
    }

private:
    void HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code &error)
    {
        if (!error)
        {
            new_session->Start();
            lock_guard<mutex> lock(_mutex);
            _session.insert(make_pair(new_session->GetUuid(), new_session));
        }
        else
        {
            cout << "session accept failed,error is " << error.what() << endl;
        }
    }
    void StartAccept()
    {
        auto &_io_context = AsioIOServicePool::GetInstance().GetIOService();
        auto new_session = make_shared<CSession>(_io_context, this);
        _acceptor.async_accept(new_session->GetSocket(), [this, new_session](auto error)
                               { HandleAccept(new_session, error); });
    }
    boost::asio::io_context &_io_context;
    short _port;
    tcp::acceptor _acceptor;
    std::map<string, shared_ptr<CSession>> _session;
    std::mutex _mutex;
};