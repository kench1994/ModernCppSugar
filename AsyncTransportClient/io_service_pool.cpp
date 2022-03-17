#include <stdafx.h>
#include "io_service_pool.h"
#include <boost/thread.hpp>
using namespace  boost::asio;
io_service_pool::io_service_pool(int thread_count)
{
	assert(thread_count);
	m_ThreadCount = thread_count;
	m_spIOService = boost::make_shared<io_service>();
	m_spIOWork = boost::make_shared<io_service::work>(*m_spIOService);
}

void io_service_pool::run()
{
	for (unsigned int i = 0; i < m_ThreadCount; ++i)
		boost::thread(boost::bind(&boost::asio::io_service::run, m_spIOService));
}

void io_service_pool::stop()
{
	m_spIOService->stop();
}

boost::shared_ptr<boost::asio::io_service> io_service_pool::get_io_service()
{
	return m_spIOService;
}

io_service_pool::~io_service_pool()
{
}
