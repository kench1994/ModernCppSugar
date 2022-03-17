#pragma once
#include <boost/thread.hpp>
typedef boost::shared_mutex READ_WRITE_MUTEX;
typedef boost::shared_lock<READ_WRITE_MUTEX> Read_Lock;
typedef boost::unique_lock<READ_WRITE_MUTEX> Write_Lock;


#define PACKAGE_HEAD char(0x1a)
#define PACKAGE_TAIL char(0x7e)

enum PACK_TYPE
{
	µ«¬º—È÷§ = 0,
	√¸¡Ó––
};