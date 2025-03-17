#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

// tcp::v4() = 0.0.0
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :_ioc(ioc),
	_acceptor(ioc, tcp::endpoint(tcp::v4(), port)) /* ,_socket(ioc)*/ 
{

}

void CServer::Start()
{
    auto self = shared_from_this();
    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_connection = std::make_shared<HttpConnection>(io_context);
    _acceptor.async_accept(new_connection->GetSocket(), [self,new_connection](beast::error_code ec) {
        try {
            //���������������ӣ���������������
            if (ec) {
                self->Start();
                return;
            }

            //���������ӣ�����HpptConnection�����������
			new_connection->Start();
            //��������
            self->Start();
        }
        catch (std::exception& exp) {
            std::cout << "exception is " << exp.what() << std::endl;
            self->Start();
        }
    });
}