#pragma once
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
class AsioThreadPool
{
public:
	explicit AsioThreadPool(size_t size)
		: m_to_work(m_io_service)
	{
		for (size_t i = 0; i < size; ++i) {
			m_trdWorkerGroup.create_thread(
				boost::bind(&boost::asio::io_service::run, &m_io_service));
		}
	}
	~AsioThreadPool()
	{
		m_io_service.stop();
		m_trdWorkerGroup.join_all();
	}
	// Add new work item to the pool.
	template<class T>
	void push_back(T t) {
		m_io_service.post(t);
	}
private:
	boost::thread_group m_trdWorkerGroup;
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_to_work;
};

