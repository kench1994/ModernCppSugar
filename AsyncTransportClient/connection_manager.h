#pragma once
#include <queue>
#include <boost/asio.hpp>
#include "async_transport.h"
#include <boost/function.hpp>


/*
连接管理类,设计为支持多连接,每个连接拥有自己的"Name",
使用者通过连接名实现对连接的管理
*/
class connection_manager 
{
public:
	connection_manager(boost::shared_ptr<boost::asio::io_service>spIOService);
	~connection_manager();


	bool create_connection(const std::string & connect_id, std::string &err_info);

	bool remove_connection(const std::string& connect_id, std::string& err_info);
	
	bool connect_remote(const std::string& connect_id, const std::string &host, int port, std::string &err_info);

	bool async_send_data(const std::string& connect_id, const std::string& orifin_data, std::string& err_info, unsigned int type = 1);

	void set_conn_state_changed(boost::function<void(const std::string& conn_id, bool bIsConnected, const std::string& err_info)>);

	void set_obtain_a_package(boost::function<void(const std::string& conn_id, const std::string& recv_data)>);

	const boost::shared_ptr<CONNECTION_BLOCK> get_conection(const std::string& connect_id, std::string& err_info);


protected:

	boost::shared_ptr<boost::asio::io_service> m_spIOService;

	boost::function<void(const std::string& conn_id, bool bConnected, const std::string& err_info)> m_funcConnStateChange;


private:
	std::map<std::string, boost::shared_ptr<CONNECTION_BLOCK>>m_mapConnectionMan;
	async_transport m_transport;
};

