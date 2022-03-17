#pragma once
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
/*
asio线程池
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
	//work对象使io_service run即使在没有任务的情况下也不会返回,直至work对象被销毁
	boost::shared_ptr <boost::asio::io_service::work> m_spIOWork;
	unsigned int m_ThreadCount = 0;
};