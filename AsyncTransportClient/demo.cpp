// AsyncTransport.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//��������״̬��仯�ص�����
	spConnectManager->set_conn_state_changed([](const std::string& conn_id, bool bIsConnected, const std::string& info) {
		std::cout << conn_id << (bIsConnected ? " Connected " : " Disconnected ") << info << std::endl;
	});

	//���ý����õ�һ����֮����÷���
	spConnectManager->set_obtain_a_package([] (const std::string& conn_id, const std::string& recv_data) {
		std::cout << conn_id << "recv a package " << recv_data << std::endl;
	});

	//�������ؽڵ�
	spConnectManager->create_connection("Express", err_info);
	//����Զ������
	spConnectManager->connect_remote("Express", "127.0.0.1", 9966, err_info);

	while (1)
		;
	return 0;
}

