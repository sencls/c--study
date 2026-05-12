#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <string>
#include <memory>
#include "../../third_party/json/json.h"
#include "../../third_party/json/value.h"
#include "../../third_party/json/reader.h"
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace my_program_state
{
    std::size_t request_count()
    {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t now()
    {
        return std::time(0);
    }
}
class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
    http_connection(tcp::socket socket) : socket_(std::move(socket))
    {
    }
    void start()
    {
        read_request();
        check_deadline();
    }

private:
    tcp::socket socket_;
    beast::flat_buffer buffer_{8192};
    http::request<http::dynamic_body> request_;
    http::response<http::dynamic_body> response_;

    net::steady_timer deadline_{socket_.get_executor(), std::chrono::seconds(60)};

    void read_request()
    {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, request_, [self](beast::error_code ec, std::size_t /* size */)
                         {
            if(!ec)
            {
                self->process_request();
            } });
    }
    void check_deadline()
    {
        auto self = shared_from_this();
        deadline_.async_wait([self](boost::system::error_code ec)
                             {
            if(!ec)
            {
                self->socket_.close(ec); 
            } });
    }
    void process_request()
    {
        response_.version(request_.version());
        response_.keep_alive(false);
        switch (request_.method())
        {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            create_response();
            break;
        case http::verb::post:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            create_post_response();
            break;
        default:
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "Invalid requset-method '" << std::string(request_.method_string()) << "'";
            break;
        }
    }
    void create_response()
    {
        if (request_.target() == "/count")
        {
            response_.set(http::field::content_type, "text/html");
            beast::ostream(response_.body()) << "<html>\n"
                                             << "<head><title>Request Count</title></head>\n"
                                             << "<body>\n"
                                             << "<h1>Request Count</h1>\n"
                                             << "<p>The number of requests received is: " << my_program_state::request_count() << "</p>\n"
                                             << "</body>\n"
                                             << "</html>\n";
        }
        else if (request_.target() == "/time")
        {
            response_.set(http::field::content_type, "text/html");
            beast::ostream(response_.body()) << "<html>\n"
                                             << "<head><title>Current Time</title></head>\n"
                                             << "<body>\n"
                                             << "<h1>Current Time</h1>\n"
                                             << "<p>The current time is: " << my_program_state::now() << "</p>\n"
                                             << "</body>\n"
                                             << "</html>\n";
        }
        else
        {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "File not found\r\n";
        }
    }

    void write_response()
    {
        auto self = shared_from_this();
        response_.content_length(response_.body().size());
        http::async_write(socket_, response_, [self](beast::error_code ec, std::size_t /*size */)
                          { self->socket_.shutdown(tcp::socket::shutdown_send, ec); 
                        self->deadline_.cancel(); });
    }
    void create_post_response()
    {
        if (request_.target() == "/email")
        {
            auto &body = this->request_.body();
            auto body_str = boost::beast::buffers_to_string(body.data());
            std::cout << "receive body is " << body_str << std::endl;
            this->response_.set(http::field::content_type, "text/json");
            Json::Value root;
            Json::Reader reader;
            Json::Value src_root;
            bool parse_success = reader.parse(body_str, src_root);
            if (!parse_success)
            {
                std::cout << "Failed to parse Json data" << std::endl;
                root["error"] = 1001;
                std::string jsonstr = root.toStyledString();
                beast::ostream(response_.body()) << jsonstr;
                return;
            }

            auto email = src_root["email"].asString();
            root["error"] = 0;
            root["email"] = src_root["email"];
            root["msg"] = "receive email post success";
            std::string jsonstr = root.toStyledString();
            beast::ostream(response_.body()) << jsonstr;
        }
        else
        {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "File not found\r\n";
        }
    }
};

void http_server(tcp::acceptor &acceptor)
{
    acceptor.async_accept(acceptor.get_executor(), [&](beast::error_code ec, tcp::socket socket)
                          {
        if(!ec)
        {
            std::make_shared<http_connection>(std::move(socket))->start();
        }
        http_server(acceptor); });
}

int main()
{

    try
    {
        auto const address = net::ip::make_address("127.0.0.1");
        unsigned short port = static_cast<unsigned short>(8080);
        net::io_context ioc(1);
        tcp::acceptor acceptor{ioc, {address, port}};
        http_server(acceptor);
        ioc.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}