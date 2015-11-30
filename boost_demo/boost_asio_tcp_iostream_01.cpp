/*
 * boost_asio_tcp_iostream_01.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description :asio 流操作,asio库提供iostream类来简化socket通信，内部集成resolver和acceptor的功能
 */
#include"std.hpp"
using namespace std;

#define BOOST_ASIO_DISABLE_STD_CHRONO
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
//#include <boost/asio/use_future.hpp>
using namespace boost::asio;
using namespace boost::system;

//////////////////////////////////////////
//使用流操作的客户端
static void client_demo()
{
	for (int i = 0; i < 5; i++)
	{
		//连接到本机6688
		ip::tcp::iostream tcp_stream("127.0.0.1", "6688");
		string str;
		getline(tcp_stream, str); //从tcp流中读取一行数据
		cout << str << endl;
	}
}
//使用流操作的服务器
static void server_demo()
{
	io_service io;

	ip::tcp::endpoint ep(ip::tcp::v4(), 6688);
	ip::tcp::acceptor acceptor(io, ep);

	for (;;)
	{
		ip::tcp::iostream tcp_stream;
		acceptor.accept(*tcp_stream.rdbuf());
		tcp_stream << "hello tcp stream";
	}
}

