#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <queue>
using namespace boost;
const int RECVSIZE = 1024;

// 封装Node结构，用来管理发送和接受的数据，包含数据的首地址、总长度及已经处理的长度
class MsgNode
{
public:
    MsgNode(const char *msg, int total_len) : _total_len(total_len), _cur_len(0)
    {
        _msg = new char[total_len];
        memcpy(_msg, msg, total_len);
    }
    MsgNode(int total_len) : _total_len(total_len), _cur_len(0)
    {
        _msg = new char[total_len];
    }
    ~MsgNode()
    {
        delete[] _msg;
    }

    int _total_len;
    int _cur_len;
    char *_msg;
};
class Session
{
public:
    Session(std::shared_ptr<asio::ip::tcp::socket> socket) : _socket(socket), _send_pending(false), _recv_pending(false) {}
    void Connet(const asio::ip::tcp::endpoint &ep)
    {
        _socket->connect(ep);
    }
    // 错误的写法，存在异步时，发送结果会混乱
    //  void WriteCallBack(const boost::system::error_code &ec, std::size_t bytes_transferred, std::shared_ptr<MsgNode> msg_node)
    //  {
    //      if (bytes_transferred + msg_node->_cur_len < msg_node->_total_len)
    //      {
    //          _send_node->_cur_len += bytes_transferred;
    //          this->_socket->async_write_some(asio::buffer(_send_node->_msg + _send_node->_cur_len, _send_node->_total_len - _send_node->_cur_len),
    //                                          [this, send_node = _send_node](const boost::system::error_code &ec, std::size_t bytes_transferred)
    //                                          {
    //                                              WriteCallBack(ec, bytes_transferred, send_node);
    //                                          });
    //      }
    //  }
    //  void WriteToSocket(const std::string buf)
    //  {
    //      _send_node = std::make_shared<MsgNode>(buf.data(), buf.size());
    //      this->_socket->async_write_some(asio::buffer(_send_node->_msg, _send_node->_total_len), [this, send_node = _send_node](const boost::system::error_code &ec, std::size_t bytes_transferred)
    //                                      { WriteCallBack(ec, bytes_transferred, send_node); });
    //  }
    void WriteCallBack(const boost::system::error_code &ec, std::size_t bytes_transferred)
    {
        if (ec.value() != 0)
        {
            std::cout << "Error code is: " << ec.value() << ". Message is: " << ec.message() << std::endl;
            return;
        }

        auto &send_data = _send_queue.front();
        send_data->_cur_len += bytes_transferred;
        if (send_data->_cur_len < send_data->_total_len)
        {
            this->_socket->async_write_some(asio::buffer(send_data->_msg + send_data->_cur_len, send_data->_total_len - send_data->_cur_len),
                                            [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
                                            { WriteCallBack(ec, bytes_transferred); });
            return;
        }
        _send_queue.pop();
        if (_send_queue.empty())
        {
            _send_pending = false;
        }
        else
        {
            auto &send_data = _send_queue.front();
            send_data->_cur_len += bytes_transferred;
            if (send_data->_cur_len < send_data->_total_len)
            {
                this->_socket->async_write_some(asio::buffer(send_data->_msg + send_data->_cur_len, send_data->_total_len - send_data->_cur_len),
                                                [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
                                                { WriteCallBack(ec, bytes_transferred); });
                return;
            }
        }
    }
    void WriteToSocket(const std::string &buf)
    {
        _send_queue.emplace(std::make_shared<MsgNode>(new MsgNode(buf.data(), buf.size())));
        if (_send_pending)
            return;
        this->_socket->async_write_some(asio::buffer(buf),
                                        [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
                                        { WriteCallBack(ec, bytes_transferred); });
        _send_pending = true;
    }

    void WriteAllCallBack(const boost::system::error_code &ec, std::size_t bytes_transferred)
    {
        if (ec.value() != 0)
        {
            std::cout << "Error code is: " << ec.value() << ". Message is: " << ec.message() << std::endl;
            return;
        }

        _send_queue.pop();
        if (_send_queue.empty())
        {
            _send_pending = false;
        }
        else
        {
            auto &send_data = _send_queue.front();
            this->_socket->async_send(asio::buffer(send_data->_msg + send_data->_cur_len, send_data->_total_len - send_data->_cur_len),
                                      [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
                                      { WriteCallBack(ec, bytes_transferred); });
        }
    }
    void WriteAllToSocket(const std::string &buf)
    {
        _send_queue.emplace(std::make_shared<MsgNode>(new MsgNode(buf.data(), buf.size())));
        if (_send_pending)
            return;
        this->_socket->async_send(asio::buffer(buf), [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
                                  { WriteCallBack(ec, bytes_transferred); });
        _send_pending = true;
    }

    void ReadFromSocket()
    {
        if (_recv_pending)
            return;

        _recv_node = std::make_shared<MsgNode>(RECVSIZE);
        _socket->async_read_some(asio::buffer(_recv_node->_msg, _recv_node->_total_len),
                                 [this](const boost::system::error_code &ec, std::size_t byte)
                                 { ReadCallBack(ec, byte); });

        _recv_pending = true;
    }
    void ReadCallBack(const boost::system::error_code &ec, std::size_t bytes_transferred)
    {
        _recv_node->_cur_len += bytes_transferred;
        if (_recv_node->_cur_len < _recv_node->_total_len)
        {
            _socket->async_read_some(asio::buffer(_recv_node->_msg + _recv_node->_cur_len, _recv_node->_total_len - _recv_node->_cur_len),
                                     [this](const boost::system::error_code &ec, std::size_t byte)
                                     { ReadCallBack(ec, byte); });
            return;
        }
        _recv_pending = false;
    }

    void ReadAllFromSocket()
    {
        if (_recv_pending)
        {
            return;
        }
        _recv_node = std::make_shared<MsgNode>(RECVSIZE);
        _socket->async_receive(asio::buffer(_recv_node->_msg, _recv_node->_total_len),
                               [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
                               {
                                   ReadAllCallBack(ec, bytes_transferred);
                               });
        _recv_pending = true;
    }
    void ReadAllCallBack(const boost::system::error_code &ec, std::size_t bytes_transferred)
    {
        _recv_node->_cur_len += bytes_transferred;
        _recv_node = nullptr;
        _recv_pending = false;
    }

private:
    bool _send_pending;
    std::shared_ptr<asio::ip::tcp::socket> _socket;
    std::shared_ptr<MsgNode> _send_node;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;

    std::shared_ptr<MsgNode> _recv_node;
    bool _recv_pending;
};
int main()
{
    return 0;
}