//============================================================================
// Name        : string_utils.h
// Author      : panzg & HondaDai
// Version     : v1
// Copyright   : 参考-->https://github.com/HondaDai/StringUtils
// Description :string_utils,c++字符串的常见操作的打包，分割，查找，删除，读文件，写文件等
//============================================================================
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
/*删除多余的空白符*/
static inline std::vector<std::string> Compact(
		const std::vector<std::string> &tokens)
{
	std::vector<std::string> compacted;

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (!tokens[i].empty())
		{
			compacted.push_back(tokens[i]);
		}
	}

	return compacted;
}
/*
 *Split:分割字符串
 *@str: 源字符串
 *@delim:分割符
 *@trim_empty:是否删除多余空白符
 *@return: 容器vector存储的分割结果
 */
static inline std::vector<std::string> Split(const std::string &str,
		const std::string &delim, const bool trim_empty = false)
{
	size_t pos, last_pos = 0, len;
	std::vector<std::string> tokens;

	while (true)
	{
		pos = str.find(delim, last_pos);
		if (pos == std::string::npos)
		{
			pos = str.size();
		}

		len = pos - last_pos;
		if (!trim_empty || len != 0)
		{
			tokens.push_back(str.substr(last_pos, len));
		}

		if (pos == str.size())
		{
			break;
		}
		else
		{
			last_pos = pos + delim.size();
		}
	}

	return tokens;
}

static inline std::string Join(const std::vector<std::string> &tokens,
		const std::string &delim, const bool trim_empty = false)
{
	if (trim_empty)
	{
		return Join(Compact(tokens), delim, false);
	}
	else
	{
		std::stringstream ss;
		for (size_t i = 0; i < tokens.size() - 1; ++i)
		{
			ss << tokens[i] << delim;
		}
		ss << tokens[tokens.size() - 1];

		return ss.str();
	}
}
//删除前后的空白符
static inline std::string Trim(const std::string &str)
{

	std::string blank = "\r\n\t ";
	size_t begin = str.size(), end = 0;
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (blank.find(str[i]) == std::string::npos)
		{
			begin = i;
			break;
		}
	}

	for (size_t i = str.size(); i > 0; --i)
	{
		if (blank.find(str[i - 1]) == std::string::npos)
		{
			end = i - 1;
			break;
		}
	}

	if (begin >= end)
	{
		return "";
	}
	else
	{
		return str.substr(begin, end - begin + 1);
	}
}
//返回str重复times次数的字符串结果
static inline std::string Repeat(const std::string &str, unsigned int times)
{
	std::stringstream ss;
	for (unsigned int i = 0; i < times; ++i)
	{
		ss << str;
	}
	return ss.str();
}
//将源字符串中target替换未replacement
static inline std::string ReplaceAll(const std::string &source,
		const std::string &target, const std::string &replacement)
{
	return Join(Split(source, target, false), replacement, false);
}
//大写转换
static inline std::string ToUpper(const std::string &str)
{
	std::string s(str);
	std::transform(s.begin(), s.end(), s.begin(), toupper);
	return s;
}
//小写转换
static inline std::string ToLower(const std::string &str)
{
	std::string s(str);
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return s;
}
//读文件
static inline std::string ReadFile(const std::string &filepath)
{
	//std::ifstream ifs(filepath.c_str());
	std::ifstream ifs(filepath); /*c++11支持*/
	if (!ifs)
	{
		std::cout << "canot open file:" << filepath << std::endl;
		return nullptr;
	}
	std::string content((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
	ifs.close();
	return content;
}
//写文件,默认为out输出模式
static inline void WriteFile(const std::string &filepath,
		const std::string &content, std::ios_base::openmode model=std::ofstream::out)
{
	//std::ofstream ofs(filepath.c_str(),model);
	std::ofstream ofs(filepath, model); //c++11支持
	ofs << content;
	ofs.close();
	return;
}

#endif
