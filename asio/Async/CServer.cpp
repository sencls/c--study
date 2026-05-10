#include "CServer.h"
#include <iostream>
#include "IOServicePool.h"
CServer::CServer(boost::asio::io_context &ioc, short port)
    : _ioc(ioc), _port(port), _acceptor(_ioc, tcp::endpoint(tcp::v4(), port))
{
    std::cout << "Server start success,listen on port " << _port << std::endl;
    StartAccept();
}

void CServer::ClearSession(std::string uuid)
{
    _session.erase(uuid);
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code &ec)
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

void CServer::StartAccept()
{
    // auto &io_context = IOServicePool::GetInstacne()->GetIOService();//使用IOServicePool
    // std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(_ioc, this);
    _acceptor.async_accept(new_session->GetSocket(), [this, new_session](const boost::system::error_code &ec)
                           { HandleAccept(new_session, ec); });
}
