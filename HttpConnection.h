#include "const.h"


class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
    
public:
    friend class LogicSystem;
    //HttpConnection(tcp::socket socket);
    HttpConnection(boost::asio::io_context& ioc);
    void Start();
    tcp::socket& GetSocket();

private:
	//检查超时
    void CheckDeadline();
	//写入响应
    void WriteResponse();
    void HandleReq();
	void PreParseGetParam();
    tcp::socket  _socket;

    // The buffer for performing reads.
    beast::flat_buffer  _buffer{ 8192 };

    // The request message.
    http::request<http::dynamic_body> _request;

    // The response message.
    http::response<http::dynamic_body> _response;

    
    // The timer for putting a deadline on connection processing.
    // 60s超时时间
    net::steady_timer deadline_{
        _socket.get_executor(), std::chrono::seconds(60) 
    };

    std::string _get_url;
    std::unordered_map<std::string, std::string> _get_params;
};