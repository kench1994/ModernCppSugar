#pragma once
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
/*
asio�̳߳�
*/
class io_service_pool
	: private boost::noncopyable
{
public:
	explicit io_service_pool(int thread_count = 1);
	~io_service_pool();

	void run();
	void stop();
	boost::shared_ptr<boost::asio::io_service> get_io_service();

private:
	boost::shared_ptr<boost::asio::io_service> m_spIOService;
	//work����ʹio_service run��ʹ��û������������Ҳ���᷵��,ֱ��work��������
	boost::shared_ptr <boost::asio::io_service::work> m_spIOWork;
	unsigned int m_ThreadCount = 0;
};