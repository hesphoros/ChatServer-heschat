#pragma once
#include "const.h"


struct UserInfo {
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};



class MySqlPool {
public:
    MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize)
        : url_(url), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize), b_stop_(false) {
        try {
            for (int i = 0; i < poolSize_; ++i) {
                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                std::unique_ptr<sql::Connection> con(driver->connect(url_, user_, pass_));
                con->setSchema(schema_);
                // 打印连接成功信息
                std::cout << "Connection successful! url: " << url_ << "user" << user_ 
                        << "password" << pass_ << "schema_" << schema_ <<std::endl;
                // 如果 URL 中包含端口信息，可以解析并打印
                size_t portPos = url_.find_last_of(':');
                if (portPos != std::string::npos) {
                    std::string port = url_.substr(portPos + 1);
                    std::cout << "Port: " << port << std::endl;
                }


                pool_.push(std::move(con));
            }
        }
        catch (sql::SQLException& e) {
            // 处理异常
            std::cout << "mysql pool init failed" << std::endl;
            // 处理异常
            std::cout << "MySQL pool initialization failed: " << e.what() << std::endl;
            std::cout << "URL: " << url_ << std::endl;
            std::cout << "User: " << user_ << std::endl;
            std::cout << "Password: " << pass_ << std::endl;

            // 如果 URL 中包含端口信息，可以解析并打印
            size_t portPos = url_.find_last_of(':');
            if (portPos != std::string::npos) {
                std::string port = url_.substr(portPos + 1);
                std::cout << "Port: " << port << std::endl;
            }
        }
    }
    std::unique_ptr<sql::Connection> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !pool_.empty(); });
        if (b_stop_) {
            return nullptr;
        }
        std::unique_ptr<sql::Connection> con(std::move(pool_.front()));
        pool_.pop();
        return con;
    }
    void returnConnection(std::unique_ptr<sql::Connection> con) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        pool_.push(std::move(con));
        cond_.notify_one();
    }
    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }
    ~MySqlPool() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            pool_.pop();
        }
    }
private:
    std::string url_;
    std::string user_;
    std::string pass_;
    std::string schema_;
    int poolSize_;
    std::queue<std::unique_ptr<sql::Connection>> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> b_stop_;
};

class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& name, const std::string& newpwd);
    bool CheckPwdWithEmail(const std::string& email, const std::string& pwd, UserInfo& userInfo);
    bool CheckPwdWithUsername(const std::string& name, const std::string& pwd, UserInfo& userInfo); 
private:
    std::unique_ptr<MySqlPool> pool_;
};