#include <stdafx.h>
#include "connection_manager.h"
using namespace boost::asio;
connection_manager::connection_manager(boost::shared_ptr<boost::asio::io_service>spIOService)
{
	assert(spIOService);
	m_spIOService = spIOService;
}


connection_manager::~connection_manager()
{
}


bool connection_manager::create_connection(const std::string & connect_id, std::string &err_info)
{
	auto& itConnect = get_conection(connect_id, err_info);
	if (itConnect)
	{
		err_info = "已存在同名连接";
		return false;
	}
	auto spConnectionBlock = boost::make_shared<CONNECTION_BLOCK>();
	spConnectionBlock->connect_id = connect_id;
	spConnectionBlock->spSocket = boost::make_shared<ip::tcp::socket>(*m_spIOService);
	spConnectionBlock->spRwMutexSendQueue = boost::make_shared<READ_WRITE_MUTEX>();
	spConnectionBlock->spSendQueue = boost::make_shared<std::queue<std::string>>();
	spConnectionBlock->funcConnectStateChange = m_funcConnStateChange;
	m_mapConnectionMan.insert(std::make_pair(std::move(connect_id), std::move(spConnectionBlock)));
	return true;
}

const boost::shared_ptr<CONNECTION_BLOCK> connection_manager::get_conection(const std::string& connect_id, std::string& err_info)
{
	if (m_mapConnectionMan.empty())
		return nullptr;
	auto& itConnect = m_mapConnectionMan.find(connect_id);
	if (itConnect == m_mapConnectionMan.end() && !m_mapConnectionMan.empty())
	{
		err_info = "未找到标识为" + connect_id + "的连接";
		return nullptr;
	}
	return itConnect->second;
}

bool connection_manager::connect_remote(const std::string& connect_id, const std::string &host_ip, int port, std::string &err_info)
{
	auto& spConnect = get_conection(connect_id, err_info);
	if (!spConnect)
		return false;
	if (spConnect->bIsConnected)
	{
		err_info = "连接:" + connect_id + "已连接到远程主机,无法再与其他主机建立连接";
		return false;
	}
	
	m_transport.connect_remote_ep(spConnect, host_ip, port, err_info);
	return true;
}

bool connection_manager::async_send_data(const std::string& connect_id, const std::string& orifin_data, std::string& err_info, unsigned int type)
{
	auto& spConnect = get_conection(connect_id, err_info);
	if (!spConnect)
		return false;
	
	if (!spConnect->bIsConnected)
	{
		err_info = connect_id + "未连接到远程主机";
		return false;
	}

	m_transport.send_data_async(spConnect->spSocket, spConnect->spSendQueue, spConnect->spRwMutexSendQueue, orifin_data, err_info, type);
	return true;
}

void connection_manager::set_conn_state_changed(boost::function<void(const std::string& conn_id, bool bIsConnected, const std::string& err_info)> funcConnStateChange)
{
	m_funcConnStateChange = [this, funcConnStateChange](const std::string& conn_id, bool bIsConnected, const std::string& err_info) {
		if (!bIsConnected)
		{
			std::string temp;
			remove_connection(conn_id, temp);
		}
		funcConnStateChange(conn_id, bIsConnected, err_info);
	};
}

void connection_manager::set_obtain_a_package(boost::function<void(const std::string& conn_id, const std::string& recv_data)> funcHook)
{
	m_transport.set_obtain_a_package(funcHook);
}

bool connection_manager::remove_connection(const std::string& connect_id, std::string& err_info)
{
	auto& spConnect = get_conection(connect_id, err_info);
	if (!spConnect)
	{
		err_info = "未建立连接";
		return false;
	}
	spConnect->spSocket->close();
	m_mapConnectionMan.erase(connect_id);
	return true;
}


