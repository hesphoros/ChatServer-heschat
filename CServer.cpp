#include "CServer.h"

#include "AsioIOServicePool.h"

// tcp::v4() = 0.0.0
CServer::CServer(boost::asio::io_context& ioc, short& port) :_io_ctx(ioc), _port(port),
	_acceptor(ioc, tcp::endpoint(tcp::v4(), port)) /* ,_socket(ioc)*/ 
{
    std::cout << "Server start success, listen on port : " << _port << std::endl;
    StartAccept();
}

CServer::~CServer()
{
    std::cout <<"Server destruct listen on port" << _port << std::endl;
}


void CServer::ClearSession(std::string uuid)
{/*
	if (_sessions.find(uuid) != _sessions.end()) {

		UserMgr::GetInstance()->RmvUserSession(_sessions[uuid]->GetUserId());
	}

	{
		lock_guard<mutex> lock(_mutex);
		_sessions.erase(uuid);
	}*/
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
