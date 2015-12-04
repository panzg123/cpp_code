/*
 * conf_utils.h
 *
 *  Created on: 2015年12月4日
 *      Author: panzg
 *      Description:解析配置文件的帮助类
 */

#ifndef CONF_UTILS_H_
#define CONF_UTILS_H_

#include <map>
#include <string>
#include "string-utils.h"

class MyConf
{
private:
	std::map<std::string, std::string> pairs;
public:
	//构造函数需要指定配置文件的路径
	MyConf(std::string file_name)
	{
		std::string content = ReadFile(file_name);
		std::vector<std::string> vec = Split(content, "\n");
		for (int i = 0; i < vec.size(); ++i)
		{
			std::vector<std::string> kv = Split(vec[i], "=");
			pairs[Trim(kv[0])] = Trim(kv[1]);
		}
	}

	std::string get(std::string key)
	{
		return pairs[key];
	}

};

#endif /* CONF_UTILS_H_ */
