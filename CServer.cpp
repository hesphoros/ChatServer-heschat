#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

// tcp::v4() = 0.0.0
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :_io_ctx(ioc),
	_acceptor(ioc, tcp::endpoint(tcp::v4(), port)) /* ,_socket(ioc)*/ 
{
    std::cout << "Server start success, listen on port : " << _port << std::endl;
    StartAccept();
}

CServer::~CServer()
{
    std::cout <<"Server destruct listen on port" << _port << std::endl;
}

void CServer::Start()
{
    auto self = shared_from_this();
    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_connection = std::make_shared<HttpConnection>(io_context);
    _acceptor.async_accept(new_connection->GetSocket(), [self,new_connection](beast::error_code ec) {
        try {
            //出错则放弃这个连接，继续监听新链接
            if (ec) {
                self->Start();
                return;
            }

            //处理新链接，创建HpptConnection类管理新连接
			new_connection->Start();
            //继续监听
            self->Start();
        }
        catch (std::exception& exp) {
            std::cout << "exception is " << exp.what() << std::endl;
            self->Start();
        }
    });
}

void CServer::HandleAccept(shared_ptr<CSession> new_session , const boost::system::error_code& error)
{
	if (!error) {
		new_session->Start();
		lock_guard<mutex> lock(_mutex);
		_sessions.insert(make_pair(new_session->GetSessionId(), new_session));
	}
	else {
		cout << "session accept failed, error is " << error.what() << endl;
	}

	StartAccept();
}

void CServer::StartAccept()
{
    /// 取出io_context 
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
    // 此占位符 是为了兼容boost::system::error
    /** BOOST_ASIO_COMPLETION_TOKEN_FOR(void (boost::system::error_code)) */
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}
