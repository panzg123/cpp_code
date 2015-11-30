/*
 * boost_resolver_demo_01.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description : asio tcp resolver的简单实例
 */

#include"std.hpp"
#include<boost/asio.hpp>
#include<boost/lexical_cast.hpp>
using namespace std;
using namespace boost::asio;
using namespace boost::system;
//根据网址和端口解析一个可用连接
void resolve_connect(ip::tcp::socket &sock,   //socket对象
		const char*name,int port)                              //网址和端口
{
	ip::tcp::resolver r(sock.get_io_service());            //resolver对象
	ip::tcp::resolver::query q(name,boost::lexical_cast<string>(port)); //创建query对象

	auto iter = r.resolve(q); //使用resolve迭代端点
	decltype(iter) end;//逾尾迭代器
	error_code ec=error::host_not_found;
	for(;ec&&iter!=end;++iter)
	{
		sock.close();
		sock.connect(*iter,ec);                          //尝试连接，成功会让ec=0
	}
	if(ec)
	{
		cout<<"can't connect"<<endl;
		throw  system_error(ec);
	}
	cout<<"connect success."<<endl;
}

//测试解析baidu的ip
static void case1()
{
	try
	{
		io_service io;
		ip::tcp::socket sock(io);
		resolve_connect(sock,"www.baidu.com",80);
		cout<<sock.remote_endpoint()<<endl;
	}
	catch (std::exception  &e)
	{
		cout<<e.what()<<endl;
	}
}

#if 0
int main()
{
	case1();
}
#endif
