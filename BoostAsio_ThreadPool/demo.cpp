#include "stdafx.h"
#include "AsioThreadPool.h"
#include <boost/atomic.hpp>
int main()
{
	AsioThreadPool pool(4);

	// Queue a bunch of work items.
	int i;
	for (i = 1; i < 8; ++i) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		pool.push_back([i]() {
			std::cout << "\r\nCurrent Working node:" << i << "\r\n" << std::endl;
			boost::this_thread::sleep(boost::posix_time::seconds(2));
		});
	}
	getchar();
	return 0;
}