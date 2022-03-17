#pragma once
#include <boost/asio.hpp>
#include <queue>
#include "package_parser.h"
typedef struct  tagConnectionBlock
{
	bool bIsConnected = false;
	std::string connect_id = "";
	boost::shared_ptr<boost::asio::ip::tcp::socket> spSocket = nullptr;
	boost::shared_ptr<std::queue<std::string>> spSendQueue = nullptr;
	boost::shared_ptr<READ_WRITE_MUTEX> spRwMutexSendQueue = nullptr;
	boost::function<void(const std::string&connect_id, bool bConnected, const std::string& info)> funcConnectStateChange;
}CONNECTION_BLOCK, *PCONNECTION_BLOCK;




class async_transport
{
public:
	async_transport();
	~async_transport();

	bool connect_remote_ep(boost::shared_ptr<CONNECTION_BLOCK> spSocket, const std::string&, int port, std::string&);


	int send_data_async(boost::shared_ptr<boost::asio::ip::tcp::socket> spSocket, boost::shared_ptr<std::queue<std::string>> spSendQueue, \
		boost::shared_ptr<READ_WRITE_MUTEX> spMutex, const std::string& data_send, std::string&, unsigned int pack_type);

	//设置接收到一个包之后调用的方法
	void set_obtain_a_package(boost::function<void(const std::string& conn_id,const std::string& recv_data)>);

private:
	package_parser m_parser;

	void handle_async_send(boost::shared_ptr<boost::asio::ip::tcp::socket> spSocket, boost::shared_ptr<std::queue<std::string>> spSendQueue, \
		boost::shared_ptr<READ_WRITE_MUTEX> spMutex);

	bool async_start_read(boost::shared_ptr<CONNECTION_BLOCK> spConnect, boost::shared_ptr<std::vector<char>> = nullptr);
	
	boost::function<void(const std::string& conn_id, const std::string& data_recv)> m_funcAfterGetAPackage;
};

