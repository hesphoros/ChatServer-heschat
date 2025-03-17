#pragma once
#include "const.h"

class HttpConnection;

typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    //处理Get
    bool HandleGet(std::string, std::shared_ptr<HttpConnection>);
    //处理Get
    void RegGet(std::string, HttpHandler handler);
    void RegPost(std::string url, HttpHandler handler);
    bool HandlePost(std::string path, std::shared_ptr<HttpConnection> con);
private:
    LogicSystem();
    std::map<std::string, HttpHandler> _post_handlers;
    std::map<std::string, HttpHandler> _get_handlers;
};
