#include <stdafx.h>
#include "async_transport.h"
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost::asio;

async_transport::async_transport()
{
	
}


async_transport::~async_transport()
{
}



bool async_transport::connect_remote_ep(boost::shared_ptr<CONNECTION_BLOCK> spConnect, const std::string &host, int port, std::string &err_info)
{
	ip::tcp::endpoint ep;
	try
	{
		ep.address(ip::address::from_string(host));
		ep.port(port);
	}
	catch (std::exception &e)
	{
		err_info = e.what();
		return false;
	}

	spConnect->spSocket->async_connect(ep,
		[this, spConnect](const boost::system::error_code& ec)  {
		auto& spConnection = spConnect;
		bool bIsConnected = false;
		std::string info;
		if (ec)
		{
			//异步连接出错
			info = ec.message();
		}
		else
		{
			//异步连接成功
			bIsConnected = true;
			spConnection->bIsConnected = true;
			info = spConnection->spSocket->remote_endpoint().address().to_string() + ":" + boost::lexical_cast<std::string>(spConnection->spSocket->remote_endpoint().port());
			async_start_read(spConnection);
		}

		if(spConnection->funcConnectStateChange)
			spConnection->funcConnectStateChange(spConnection->connect_id, bIsConnected, info);
	});
	return true;
}



int async_transport::send_data_async(boost::shared_ptr<boost::asio::ip::tcp::socket> spSocket, boost::shared_ptr<std::queue<std::string>> spSendQueue, \
	boost::shared_ptr<READ_WRITE_MUTEX> spMutex, const std::string& origin_data, std::string& err_info, unsigned int pack_type)
{

	//=============数据封包=============
	std::string package_coded;
	m_parser.package_encode(origin_data, package_coded);
	Write_Lock wLock(*spMutex);
	bool on_writting = !spSendQueue->empty();
	//将数据加入发送队列中
	spSendQueue->emplace(std::move(package_coded));
	wLock.unlock();
	//上一次发送完成才开始发送
	if (!on_writting)
	{
		handle_async_send(spSocket, spSendQueue, spMutex);
	}
	return 0;
}





void async_transport::set_obtain_a_package(boost::function<void(const std::string& conn_id, const std::string& recv_data)> funcHook)
{
	m_funcAfterGetAPackage = funcHook;
}


void async_transport::handle_async_send(boost::shared_ptr<ip::tcp::socket> spSocket, boost::shared_ptr<std::queue<std::string>> spSendQueue, \
	boost::shared_ptr<READ_WRITE_MUTEX> spMutex)
{
	Read_Lock rLock(*spMutex);
	boost::asio::async_write(*spSocket,
		boost::asio::buffer(spSendQueue->front()),
		[this, spSocket, spSendQueue, spMutex](const boost::system::error_code& ec, std::size_t size)
	{
		if (!ec)
		{
			Write_Lock wLock(*spMutex);
			spSendQueue->pop();
			wLock.unlock();
			Read_Lock rLock(*spMutex);
			if (!spSendQueue->empty())//发送结束,循环调用发送
			{
				handle_async_send(spSocket, spSendQueue, spMutex);
			}
			rLock.unlock();

		}
		else
		{
			std::cout << "On Async Send Error, ErrCode:" << ec.value() << ",ErrMsg:" << ec.message();
		}
	});
	rLock.unlock();
}

bool async_transport::async_start_read(boost::shared_ptr<CONNECTION_BLOCK> spConnect, boost::shared_ptr<std::vector<char>>spBufferPrio)
{
	if (!spConnect)
		return false;
	if (!spConnect->bIsConnected)
		return false;
	if (!spConnect->spSocket)
		return false;
	if (!spConnect->spSocket->is_open())
		return false;
	auto spIORecvBuffer = boost::make_shared<boost::array<char, 1024>>();

	spConnect->spSocket->async_read_some(boost::asio::buffer(*spIORecvBuffer),
		[this, spConnect, spIORecvBuffer, spBufferPrio](const boost::system::error_code& ec, size_t bytes_transferred) {
		if (!ec)
		{

			boost::shared_ptr<std::vector<char>> spVecBuffer(new std::vector<char>);

			if (!spBufferPrio)
			{
				spVecBuffer->resize(bytes_transferred);
				std::copy(spIORecvBuffer->begin(), spIORecvBuffer->begin() + bytes_transferred, spVecBuffer->begin());
			}
			else
			{
				size_t prio_buffer_size = spBufferPrio->size();
				spVecBuffer->resize(bytes_transferred + prio_buffer_size);
				std::copy(spBufferPrio->begin(), spBufferPrio->begin() + prio_buffer_size, spVecBuffer->begin());
				std::copy(spIORecvBuffer->begin(), spIORecvBuffer->begin() + bytes_transferred, spVecBuffer->begin() + prio_buffer_size);
			}

			unsigned int index = 0;
			while (true)
			{
				unsigned int source_length = spVecBuffer->size();//strlen(data_stream);
				const char* data_stream = &(spVecBuffer->at(index));

				if (data_stream[0] == PACKAGE_HEAD)
				{
					unsigned long body_length;
					char length_arr[5] = "";
					for (unsigned char j = 0; j < 4; ++j)
						length_arr[j] = data_stream[j + 1];
					body_length = atoi(length_arr);

					//无法组成一个完整包,继续接收
					if ((body_length + 6 + index )> source_length)
					{
						spVecBuffer->erase(spVecBuffer->begin(), spVecBuffer->begin() + index);
						spVecBuffer->resize(source_length - index);
						async_start_read(spConnect, spVecBuffer);
						return false;
					}
					else if (data_stream[body_length + 5] == PACKAGE_TAIL)
					{
						std::string single_reply;
						for (unsigned long i = 0; i < body_length; i++)
						{
							single_reply += data_stream[i + 5];
						}
						m_funcAfterGetAPackage(spConnect->connect_id, single_reply);

						index += (body_length + 6);
						if (index == spVecBuffer->size())
						{
							async_start_read(spConnect);
							return true;
						}
					}
					else
					{
						//报告包错误
						std::string error_pack;
						for (unsigned int i = 0; i < source_length; i++)
						{
							if (data_stream[i] == '/0')
								break;
							error_pack += data_stream[i];
						}
						std::cout << error_pack;
						async_start_read(spConnect);
						return false;
					}
				}
				else
				{		
					//报告包错误
					bool bFindNextPack = false;
					for (unsigned int i = 1; i < source_length; i++)
					{
						if (data_stream[i] == PACKAGE_HEAD)
						{
							index = i;
							bFindNextPack = true;
							break;
						}
					}
					if (bFindNextPack)
					{
						spVecBuffer->erase(spVecBuffer->begin(), spVecBuffer->begin() + index);
						spVecBuffer->resize(source_length - index); 
						async_start_read(spConnect, spVecBuffer);
						return false;
					}
					else
					{
						std::cout << "Package Head Not Found";
						async_start_read(spConnect);
						return false;
					}
					
				}
			}



		}
		else
		{
			if (spConnect->funcConnectStateChange)
				spConnect->funcConnectStateChange(spConnect->connect_id, false, "ErrCode:" + boost::lexical_cast<std::string>(ec.value()) + ",ErrMsg:" + ec.message());// "读错误");
			return false;
		}
		return true;
	});
	return true;
}
