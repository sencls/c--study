// 网络编程的基本流程
// 对服务端：
//     1.socket(创建socket对象)
//     2.bind(绑定本机ip+port)
//     3.listen(监听来电，若存在就建立连接)
//     4.accept(再创建一个socket对象给其他发消息 实际中要面向多个客户端,为了区分各个客户端,都需要一个socket对象收发消息)
//     5.read,write(收发消息)
// select模型,epoll模型,proactor模型,reactor模型
// 对客户端
//     1.socket(创建socket对象)
//     2.connect
//     3.read,write
#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <memory>
using namespace boost;

int client_end_point() // 客户端创建端点，将ip地址与端口号绑定在一起，生成一个端点对象
{
    std::string raw_ip_address = "127.4.8.1";                                  // 对方的IP；
    unsigned short port_num = 3333;                                            // 对方的端口号
    boost::system::error_code ec;                                              // 错误码对象
    asio::ip::address ip_address = asio::ip::make_address(raw_ip_address, ec); // 将该对象转换成ip地址string->ip地址对象
    if (ec.value() != 0)
    {
        std::cout << "failed to parse the ip address. Error code =" << ec.value() << "Message:" << ec.message() << std::endl;
        return ec.value();
    }
    asio::ip::tcp::endpoint ep(ip_address, port_num); // 生成端点
    return 0;
}

int server_end_point() // 服务器端点
{
    unsigned short port_num = 3333;
    asio::ip::address ip_address = asio::ip::address_v4::any(); // 任何地址都可以与我通信；ipv4的，address_v6
    asio::ip::tcp::endpoint ep(ip_address, port_num);
    return 0;
}

int create_tcp_socket()
{
    asio::io_context ioc;                         // 上下文对象
    asio::ip::tcp protocol = asio::ip::tcp::v4(); // 协议对象
    asio::ip::tcp::socket sock(ioc);              // 创建socket对象
    // asio::ip::tcp::socket sock(ioc, asio::ip::tcp::v4());指定协议创建

    // boost::system::error_code ec; //新版本优化，socket对象会自己判断是否打开，只要通信就会自行打开
    // sock.open(protocol, ec); // 打开socket对象
    // if (ec.value() != 0)
    // {
    //     std::cout << "failed to open the socket" << ec.value() << "message:" << ec.message() << std::endl;
    //     return ec.value();
    // }
    return 0;
}

int create_acceptor_socket() // 服务端的接受
{
    asio::io_context ios;
    asio::ip::tcp::acceptor a(ios, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3333));
    // asio::ip::tcp::acceptor acceptor(ios);//老写法
    // asio::ip::tcp protocol = asio::ip::tcp::v4();
    // boost::system::error_code ec;
    // acceptor.open(protocol, ec);
    // if (ec.value() != 0)
    // {
    //     std::cout << "failed to open the acceptor socket" << ec.value() << "message:" << ec.message() << std::endl;
    //     return ec.value();
    // }
    // return 0;
    return 0;
}

int bind_acceptor_socket() // 服务器的绑定
{
    unsigned short port = 3333;
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port);
    asio::io_context ios;
    asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
    boost::system::error_code ec;
    acceptor.bind(ep, ec);
    if (ec.value() != 0)
    {
        std::cout << "failed to open the acceptor socket" << ec.value() << "message:" << ec.message() << std::endl;
        return ec.value();
    }
    return 0;
}

int connect_to_end() // 客户端连接服务器
{
    std::string raw_ip_address = "192.168.1.124"; // 服务器的IP地址
    unsigned short port_num = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::make_address(raw_ip_address), port_num);
        asio::io_context ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}

int dns_connect_to_end() // dns解析连接服务器
{
    std::string host = "llfc.club";
    std::string port_num = "3333";
    asio::io_context ios;
    asio::ip::tcp::resolver resolver(ios);
    try
    {
        auto endpoints = resolver.resolve(host, port_num);
        asio::ip::tcp::socket sock(ios);
        asio::connect(sock, endpoints);
    }
    catch (const std::exception &e)
    {
        std::cout << "failed to resolve the host name" << e.what() << std::endl;
    }
    return 0;
}

int accept_new_connection() // 服务器接受连接
{
    const int BACKlLOG = 30; // 缓冲区队列
    unsigned short port_num = 3333;
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);
    asio::io_context ios;
    try
    {
        asio::ip::tcp::acceptor acceptor(ios, ep.protocol()); // 服务器创建接受
        acceptor.bind(ep);                                    // 服务器绑定
        acceptor.listen(BACKlLOG);                            // 服务器监听
        asio::ip::tcp::socket sock(ios);                      // 创建新的socket对象
        acceptor.accept(sock);                                // 处理客户端的消息
    }
    catch (const std::exception &e)
    {
        std::cout << "failed to accept new connection" << e.what() << std::endl;
    }
    return 0;
}

void use_const_buffer()
{
    std::string s = "hello asio";
    asio::const_buffer asio_buf(s.data(), s.size()); // s.data()返回字符串的首地址
    std::vector<asio::const_buffer> buffers_sequence;
    buffers_sequence.emplace_back(asio_buf);
}

void use_buffer_str()
{
    asio::const_buffer output_buf = asio::buffer("hello asio"); // 直接构造
}

void use_buffer_array()
{
    const size_t BUF_SIZE_BYTES = 20;
    std::unique_ptr<char[]> buf(new char[BUF_SIZE_BYTES]);
    auto input_buf = asio::buffer(static_cast<void *>(buf.get()), BUF_SIZE_BYTES);
}

void write_to_socket(asio::ip::tcp::socket &sock)
{
    std::string buf = "hello asio";
    std::size_t total_bytes_written = 0;
    while (total_bytes_written != buf.size()) // write_some()函数返回一次写入的字节数
    {
        total_bytes_written += sock.write_some(asio::buffer(buf.data() + total_bytes_written, buf.size() - total_bytes_written));
    }
}

int send_data_by_write_some()
{
    std::string raw_ip_address = "192.168.3.11";
    unsigned short port_num = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::make_address(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        write_to_socket(sock);
    }
    catch (system::system_error &e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;
}

int send_data_by_send()
{
    std::string raw_ip_address = "192.168.3.11";
    unsigned short port_num = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::make_address(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        std::string buf = "hello asio";
        int send_length = sock.send(asio::buffer(buf.data(), buf.size())); // 阻塞式的发送函数，要么全部发送，要么异常
        if (send_length <= 0)
        {
            return 0;
        }
    }
    catch (system::system_error &e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;
}

int send_data_by_write()
{
    std::string raw_ip_address = "192.168.3.11";
    unsigned short port_num = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::make_address(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        std::string buf = "hello asio";
        int send_length = asio::write(sock, asio::buffer(buf.data(), buf.size()));
        if (send_length <= 0)
        {
            return 0;
        }
    }
    catch (system::system_error &e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;
}

std::string read_from_socket(asio::ip::tcp::socket &sock)
{
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    std::size_t total_bytes_read = 0;
    while (total_bytes_read != MESSAGE_SIZE)
    {
        total_bytes_read += sock.read_some(asio::buffer(buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
    }
    return std::string(buf, total_bytes_read);
}

int read_data_by_read_some()
{
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::make_address(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        read_from_socket(sock);
    }
    catch (system::error_code &e)
    {
        std::cerr << e.what() << '\n';
    }
}

int read_data_by_receive()
{
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::make_address(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        const unsigned char MESSAGE_SIZE = 7;
        char buf[MESSAGE_SIZE];
        int receive_length = sock.receive(asio::buffer(buf, MESSAGE_SIZE));
        if (receive_length <= 0)
        {
            return 0;
        }
    }
    catch (system::error_code &e)
    {
        std::cerr << e.what() << '\n';
    }
}

int read_data_by_read()
{
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::make_address(raw_ip_address), port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        const unsigned int MESSAGE_SIZE = 7;
        char buf[MESSAGE_SIZE];
        int receive_length = asio::read(sock, asio::buffer(buf, MESSAGE_SIZE));
        if (receive_length <= 0)
        {
            return 0;
        }
    }
    catch (system::error_code &e)
    {
        std::cerr << e.what() << '\n';
    }
}

int main()
{
    client_end_point();
    return 0;
}
// 网络库中的buffer:接受和发送数据时缓存数据的结构
// boost::asio提供asio::mutable_buffet asio::const_buffer两种,他们是连续的空间,首字节存储长度
// mutable_buffer用于写服务,const_buffer用于读服务,但是这两种结构并没有直接被asio的api使用
// 对于api 提供了 MutableBufferSequence与ConstBufferSequence两种类型,它们是多个上述结构的组成
// 可以理解MutableBufferSequence为vector<asio::mutable_buffer>,内部存储的是mutable_buffer的地址
// 于是有函数buffer(),该函数接受多种形式的字节流,返回mutable_buffers_1 或者const_buffers_1对象(两者已被弃用v1.82以后)，直接返回mutable_buffer或者const_buffer对象
// 它们是MutableBufferSequence与ConstBufferSequence的实现类,内部存储了一个mutable_buffer或者const_buffer对象
