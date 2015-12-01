/*
 * p_string.h
 *
 *  Created on: 2015年12月1日
 *      Author: panzg
 *      Description:c++ string类的简单实现，参考：陈硕
 */

#include<utility>
#include<cstring>
#include<ostream>
#include<iostream>
using namespace std;

class p_string
{
public:
	//构造函数，如：p_string test;
	p_string() :
			data_(new char[1])
	{
		*data_ = '\0';
	}
	//构造函数，如：p_string test("hello p_string")
	p_string(const char* str) :
			data_(new char[strlen(str) + 1])
	{
		strcpy(data_, str);
	}
	//构造函数，p_string test2(test1)
	p_string(const p_string& rhs) :
			data_(new char[rhs.size() + 1])
	{
		strcpy(data_, rhs.c_str());
	}
	//复制
	p_string& operator=(p_string rhs)
	{
		std::swap(data_, rhs.data_);
		return *this;
	}
	//重载比较运算符
	bool operator==(const p_string rhs)
	{
		return strcmp(rhs.data_, data_) == 0;
	}
	//重载流
	friend ostream& operator<<(ostream& out, p_string& t)    //输出流重载声明及实现
	{
		cout<<t.data_;
		return out;
	}
	friend istream& operator>>(istream& in, p_string& t)      //输入流重载声明及实现
	{
		in>>t.data_;
		return in;
	}
	//析构函数
	~ p_string()
	{
		delete[] data_;
	}

	//获取数据的长度
	size_t size() const
	{
		return strlen(data_);
	}
	//返回const char*数据
	const char* c_str() const
	{
		return data_;
	}
private:
	char* data_;
};

