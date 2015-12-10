/*
 * filesystem_demo.cpp
 *
 *  Created on: 2015年12月10日
 *      Author: panzg
 */

#include"boost/filesystem.hpp"
#include"boost/optional.hpp"
#include"boost/xpressive/xpressive_dynamic.hpp"
#include"boost/algorithm/string.hpp"
#include<string>
#include<iostream>
#include<vector>
using namespace boost;
using namespace std;
using namespace boost::filesystem;
using namespace boost::xpressive;

//实现文件查找的功能
static boost::optional<path> find_file(const path&dir,
		const std::string& filename)
{
	typedef optional<path> result_type;
	if (!exists(dir) || !is_directory(dir))
		return result_type();
	recursive_directory_iterator end;
	for (recursive_directory_iterator pos(dir); pos != end; pos++)
	{
		if (!is_directory(*pos) && pos->path().filename() == filename)
			return result_type(pos->path());
	}
	return result_type();
}
//find_file函数的测试
static void find_file_test()
{
	auto r = find_file("/usr/local/boost/include", "version.hpp");
	if (r)
		cout << *r << endl;
	else
		cout << "file not found." << endl;
}

vector<path> find_regex_files(const path&dir, const string& filename)
{
	static xpressive::sregex_compiler rc; //正则表达式工厂
	if (rc[filename].regex_id())
	{
		string str = replace_all_copy(replace_all_copy(filename, ".", "\\."),
				"*", ".*");   //处理文件名
		rc[filename] = rc.compile(str); //创建正则表达式
	}
	typedef vector<path> result_type;
	result_type v;
	if (!exists(dir) || !is_directory(dir))
	{
		return v;
	}
	//递归匹配
	typedef recursive_directory_iterator rd_iterator;
	rd_iterator end;
	for (rd_iterator pos(dir); pos != end; ++pos)
	{
		if (!is_directory(*pos)
				&& regex_match(pos->path().filename().string(), rc[filename]))
		{
			v.push_back(pos->path());
		}
	}

	return v;
}
//测试
static void find_regex_files_test()
{
	auto v = find_regex_files("/usr/local/include/boost/timer", "*.hpp");
	cout << v.size() << endl;

	for (path &p : v)
	{
		cout << p << endl;
	}
}
/////////////////////////////////////////////////////
#include"boost/progress.hpp"   //boost进度条
//从源目录复制符合要求的文件到指定目录
size_t copy_files(const path&from_dir, const path& to_dir,
		const string& filename = "*")
{
	if (!is_directory(from_dir))
	{
		cout << "args is not a dir." << endl;
		return 0;
	}

	cout << "prepare for copy, please wait..." << endl;

	auto v = find_regex_files(from_dir, filename);
	if (v.empty())
	{
		cout << "0 file copied." << endl;
		return 0;
	}

	cout << "now begin copy files ..." << endl;
	path tmp;
	progress_display pd(v.size());
	for (auto &p : v)
	{
		//拆分路径，形成目标路径
		tmp = to_dir / p.string().substr(from_dir.string().length());
		//创建子目录
		if (!exists(tmp.parent_path()))
		{
			create_directories(tmp.parent_path());
		}
		//拷贝文件，copy_file是boost api
		copy_file(p, tmp);
		++pd;
	}
	cout << v.size() << " file copied." << endl;
	return v.size();
}
//测试
void copy_files_test()
{
	copy_files("/usr/local/include/boost/timer", "./t");
}
/////////////////////////////////////////////////////
#if 0
int main()
{
//bug:too many levels of symbolic links
//	copy_files_test();
}

#endif
