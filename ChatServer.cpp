#include "LogicSystem.h"
#include <csignal>
#include <thread>
#include <mutex>
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "ConfigMgr.h"


bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main()
{
	try {
		auto& cfg = ConfigMgr::Inst();
		auto pool = AsioIOServicePool::GetInstance();
		boost::asio::io_context io_cxt;
		boost::asio::signal_set signals(io_cxt, SIGINT, SIGTERM);
		signals.async_wait([&io_cxt, pool](auto, auto) {
			io_cxt.stop();
			pool->Stop();
			});

		auto port_str = cfg["SelfServer"]["Port"];
		unsigned short port = static_cast<unsigned short>(atoi(port_str.c_str()));  // 将端口号转换为 unsigned short
		CServer s(io_cxt, port);  // 传递 io_cxt 的引用和 port 变量
		io_cxt.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

}
