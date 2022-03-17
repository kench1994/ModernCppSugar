// AsyncTransport.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "io_service_pool.h"
#include "connection_manager.h"
int main()
{
	io_service_pool io_pool(4);
	io_pool.run();
	
	auto spIOService = io_pool.get_io_service();
	std::string err_info;

	auto spConnectManager = boost::make_shared<connection_manager>(spIOService);
	//设置连接状态表变化回调函数
	spConnectManager->set_conn_state_changed([](const std::string& conn_id, bool bIsConnected, const std::string& info) {
		std::cout << conn_id << (bIsConnected ? " Connected " : " Disconnected ") << info << std::endl;
	});

	//设置解析得到一个包之后调用方法
	spConnectManager->set_obtain_a_package([] (const std::string& conn_id, const std::string& recv_data) {
		std::cout << conn_id << "recv a package " << recv_data << std::endl;
	});

	//创建本地节点
	spConnectManager->create_connection("Express", err_info);
	//连接远程主机
	spConnectManager->connect_remote("Express", "127.0.0.1", 9966, err_info);

	while (1)
		;
	return 0;
}

