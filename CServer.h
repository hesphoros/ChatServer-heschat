#pragma once
#include "const.h"




class CServer :
    public std::enable_shared_from_this<CServer>
{
public:
    /// <summary>
    /// 
    /// </summary>
    /// <param name="ioc"></param> 上下文
    /// <param name="port"></param> 端口
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    ~CServer() = default;
    void Start();
private:
    //接收器
    tcp::acceptor  _acceptor;
    net::io_context& _ioc;
    //tcp::socket   _socket;
};