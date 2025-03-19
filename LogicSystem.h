#pragma once
#include "const.h"
#include "CSession.h"

class HttpConnection;

typedef  function<void(shared_ptr<CSession>, const short& msg_id, const string& msg_data)> FunCallBack;

class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    void PostMsgToQue(shared_ptr < LogicNode> msg);
    void LoginHandler(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
private:
    /// <summary>
    /// ×¢²á»Øµ÷
    /// </summary>
    void RegisterCallBacks();
    LogicSystem();
    void DealMsg();
private:
   
    std::thread                                             _worker_thread;
    std::queue<shared_ptr<LogicNode>>                       _msg_queue;
    std::condition_variable                                 _con_var;
    bool                                                    _b_stop;
    std::mutex                                              _mutex;
    std::map<short, FunCallBack>                            _fun_callbacks;

};
