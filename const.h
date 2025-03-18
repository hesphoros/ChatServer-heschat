#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include "Singleton.h"
#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <map>
#include <queue>
#include <atomic>
#include <condition_variable>

#include "hiredis.h"
#include <ws2tcpip.h>
#include <cassert>




enum ErrorCodes {
    Success = 0,
    Error_Json = 1001,  //Json 解析错误
    RPCFailed = 1002,  //RPC请求错误
	VerifyExpired = 1003,  //验证码过期
	VerifyCodeErr = 1004,  //验证码错误
	UserExist = 1005,  //用户已存在
	PasswdErr = 1006,  //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdInvalid = 1008,  //密码不合法
};

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#define CODEPREFIX "code_"

#define MAX_LENGTH  1024*2
//头部总长度
#define HEAD_TOTAL_LEN 4
//头部id长度
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE  10000
#define MAX_SENDQUE 1000

