#include <iostream>
#include <boost/asio.hpp>
#include <set>
#include <memory>
using namespace boost::asio::ip;
const int MAX_LENGTH = 1024;
typedef std::shared_ptr<tcp::socket> socket_ptr;
std::set<std::shared_ptr<std::thread>> thread_set;
void session(socket_ptr sock)
{
    try
    {
        for (;;)
        {
            char data[MAX_LENGTH];
            memset(data, 0, MAX_LENGTH);
            boost::system::error_code er;
            // size_t length = boost::asio::read(sock,boost::asio::buffer(data,MAX_LENGTH),er);
            size_t length = sock->read_some(boost::asio::buffer(data, MAX_LENGTH), er);
            if (er == boost::asio::error::eof)
            {
                std::cout << "connection closed" << std::endl;
                break;
            }
            else if (er)
            {
                throw boost::system::system_error(er);
            }
            std::cout << "received data: " << sock->remote_endpoint().address().to_string() << std::endl;
            std::cout << "receive message: " << data << std::endl;
            boost::asio::write(*sock, boost::asio::buffer(data, length));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void server(boost::asio::io_context &io_c, unsigned short port)
{
    tcp::acceptor a(io_c, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        socket_ptr socket(new tcp::socket(io_c));
        a.accept(*socket);
        auto t = std::make_shared<std::thread>(session, socket);
        thread_set.insert(t);
    }
}
int main()
{
    try
    {
        boost::asio::io_context ioc;
        server(ioc, 10086);
        for (auto &t : thread_set)
        {
            t->join();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}