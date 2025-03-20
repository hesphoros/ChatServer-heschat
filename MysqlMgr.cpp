#include "MysqlMgr.h"

MysqlMgr::~MysqlMgr() {
}
int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    return _dao.RegUser(name, email, pwd);
}
bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
    return _dao.CheckEmail(name,email);
}
bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd)
{
    return _dao.UpdatePwd(name,pwd);
}
bool MysqlMgr::CheckPwdWithUsername(const std::string& name, const std::string& pwd, UserInfo& userInfo)
{
    return _dao.CheckPwdWithUsername(name, pwd, userInfo);
}
bool MysqlMgr::CheckPwdWithEmail(const std::string& email, const std::string& pwd, UserInfo& userInfo)
{
    return _dao.CheckPwdWithEmail(email,pwd,userInfo);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return _dao.GetUser(uid);

}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
    return _dao.GetUser(name);
}

MysqlMgr::MysqlMgr() {
}