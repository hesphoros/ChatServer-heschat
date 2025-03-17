#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"



using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

//RPC 连接池
class RPConPool {
public:
    RPConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {
            std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                grpc::InsecureChannelCredentials());
            connections_.push(VerifyService::NewStub(channel));
        }
    }
    ~RPConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }
    std::unique_ptr<VerifyService::Stub> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !connections_.empty();
            });
        //如果停止则直接返回空指针
        if (b_stop_) {
            return  nullptr;
        }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }
    void returnConnection(std::unique_ptr<VerifyService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }
    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }
private:
	std::atomic<bool> b_stop_; //是否需要回收资源
	size_t poolSize_;          //连接池大小
	std::string host_;         //主机地址
	std::string port_;         //端口
	std::queue<std::unique_ptr<VerifyService::Stub>> connections_;  //连接队列

    /** 控制队列线程安全*/
    std::mutex mutex_;      
    std::condition_variable cond_;
};


class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:

    GetVerifyRsp GetVarifyCode(std::string email) {
        //上下文
        ClientContext context;
        GetVerifyRsp reply;//回包
		GetVerifyReq request;//请求
        request.set_email(email);
		auto stub_ = pool_->getConnection();
        Status status = stub_->GetVerifyCode(&context, request, &reply);

        if (status.ok()) {
			// 回收连接
			pool_->returnConnection(std::move(stub_));
            return reply;
        }
        else {
            pool_->returnConnection(std::move(stub_));
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }

private:
    VerifyGrpcClient();
    std::unique_ptr<RPConPool> pool_;
   
};

