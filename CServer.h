#pragma once

#include <boost/asio.hpp>
#include "CSession.h"
#include <memory>
#include <atomic>
#include <mutex>
#include <map>

using boost::asio::ip::tcp;

class CServer :
    public std::enable_shared_from_this<CServer>
{
public:
    /// <summary>
    /// 
    /// </summary>
    /// <param name="ioc"></param> 上下文
    /// <param name="port"></param> 端口
    CServer(boost::asio::io_context& ioc, short& port);
    ~CServer();
   
    void ClearSession(std::string);
private:
    void HandleAccept(shared_ptr<CSession>, const boost::system::error_code& error);
    void StartAccept();
private:
    //接收器
    tcp::acceptor                                        _acceptor;
    net::io_context&                                     _io_ctx;
    short                                                _port;
	std::map<std::string, shared_ptr<CSession>>          _sessions;
	std::mutex                                           _mutex;
    //tcp::socket   _socket;
};