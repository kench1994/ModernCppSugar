#pragma once
#include "config.h"
//数据包打包解压方法类
class package_parser
{
public:

	package_parser() {}
	~package_parser() {}

	void package_encode(const std::string& origin_data, std::string& after_code)
	{
		char body_length[5] = "";
		sprintf_s(body_length, sizeof(body_length), "%4d", origin_data.length());
		after_code = PACKAGE_HEAD;
		for (unsigned char i = 0; i < 4; i++)
		{
			after_code += body_length[i];
		}
		after_code += std::move(origin_data);
		after_code += PACKAGE_TAIL;
	}
};
