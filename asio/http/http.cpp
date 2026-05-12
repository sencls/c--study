/*
http请求头：
    1.request-line 包含请求类型、访问的资源、使用的http版本信息号
    2.host 被请求资源的主机号或者ip与端口
    3.accept 指定客户端能够接受的媒体类型列表，使用逗号分隔开
    4.user-agent 客户端使用的浏览器类型与版本号
    5.cookie 如果包含cookie的话，会将之发送到服务器
    6.connection 表示是否需要持久链接

http响应头：
    1.statue-line 协议版本、状态码、状态消息
    2.content-type 响应体的mime类型
    3.content-length 响应体的字节数
    4.set-cookie 服务器向客户端发送cookie消息时使用该字段
    5.server 服务器类型与版本号
    6.connection 同上
*/
#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <boost/bind/bind.hpp>
#include <filesystem>
using boost::asio::ip::tcp;

class client
{
public:
    client(boost::asio::io_context &io_context, const std::string &server, const std::string &path) : _resolver(io_context), _socket(io_context)
    {
        std::ostream request_stream(&_request);
        request_stream << "GET " << path << " HTTP/1.0\r\n";
        request_stream << "Host: " << server << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        size_t pos = server.find(":");
        std::string ip = server.substr(0, pos);
        std::string port = server.substr(pos + 1);

        _resolver.async_resolve(ip, port, [this](auto er, auto endpoint)
                                { handle_resolve(er, endpoint); });
    }
    void handle_resolve(const boost::system::error_code &er, const tcp::resolver::results_type &endpoints)
    {
        if (!er)
        {
            boost::asio::async_connect(_socket, endpoints, [this](auto er, auto b)
                                       { handle_connect(er); });
        }
        else
        {
            std::cout << "Error: " << er.message() << std::endl;
        }
    }
    void handle_connect(const boost::system::error_code &ec)
    {
        if (!ec)
        {
            boost::asio::async_write(_socket, _request, [this](auto er, auto b)
                                     { handle_write_request(er); });
        }
        else
        {
            std::cout << "Error: " << ec.message() << std::endl;
        }
    }

    void handle_write_request(const boost::system::error_code &er)
    {
        if (!er)
        {
            boost::asio::async_read_until(socket, _response, "\r\n", [this](auto er, auto b)
                                          { handle_read_status_line(er); });
        }
        else
        {
            std::cout << "Error: " << er.message() << std::endl;
        }
    }
    void handle_read_status_line(const boost::system::error_code &er)
    {
        if (!er)
        {
            std::istream response_stream(&_response);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                std::cout << "invalid response\n";
            }
            if (status_code != 200)
            {
                std::cout << "Response returned with status code: " << status_code << '\n';
                return;
            }
            boost::asio::async_read_until(_socket, _response, "\r\n\r\n", [this](auto ec, auto b)
                                          { handle_read_headers(ec); });
        }
        else
        {
            std::cout << "Error: " << er.message() << std::endl;
        }
    }

    void handle_read_headers(const boost::system::error_code &ec)
    {
        if (!ec)
        {
            std::istream response_stream(&_response);
            std::string header;
            while (std::getline(response_stream, header) && header != "\r")
            {
                std::cout << header << "\n\n";
            }

            if (_response.size() > 0)
            {
                std::cout << &_response;
            }

            boost::asio::async_read(_socket, _response, boost::asio::transfer_at_least(1), [this](auto ec, auto b)
                                    { handle_read_content(ec); }); // 至少要读一个字符；
        }
        else
        {
            std::cout << "Error: " << ec.message() << std::endl;
        }
    }

    void handle_read_content(boost::system::error_code &ec)
    {
        if (!ec)
        {
            std::cout << &_response;

            boost::asio::async_read(_socket, _response, boost::asio::transfer_at_least(1), [this](auto ec, auto b)
                                    { handle_read_content(ec); });
        }
        else
        {
            std::cout << "Error: " << ec.message() << std::endl;
        }
    }

private:
    tcp::resolver _resolver;
    tcp::socket _socket;
    boost::asio::streambuf _request;
    boost::asio::streambuf _response;
};

int main(int argc, char *argv[])
{
    try
    {

        boost::asio::io_context ioc;
        client(ioc, "127.0.0.1:8080", "/");
        ioc.run();
        getchar();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
