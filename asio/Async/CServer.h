#pragma once
#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <string>
#include "CSession.h"

using boost::asio::ip::tcp;

class CServer
{
public:
    CServer(boost::asio::io_context &ioc, short port);
    void ClearSession(std::string uuid);

private:
    void HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code &ec);
    void StartAccept();

    boost::asio::io_context &_ioc;
    short _port;
    tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<CSession>> _session;
};
