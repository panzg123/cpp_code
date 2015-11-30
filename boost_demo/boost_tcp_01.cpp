/*
 * boost_tcp_01.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description : tcp类的demo
 */

#include<boost/asio.hpp>
#include"std.hpp"
#include<boost/asio/ip/tcp.hpp>

using namespace boost::asio;
using namespace boost::system;
//address类的用法
//static表示只在本文件可见，否则出现重复定义
static void case1()
{
	ip::address addr;
	addr = addr.from_string("127.0.0.1");
	cout << addr.is_v4() << endl;
	cout << addr.to_string() << endl;

	addr = addr.from_string("ab::124:34:56");
	cout << addr.is_v6() << endl;
}
//endpoint类的用法
//static表示只在本文件可见，否则出现重复定义
static void case2()
{
	ip::address addr;
	addr = addr.from_string("127.0.0.1");

	ip::tcp::endpoint ep(addr, 6688);
	cout << ep.address() << "   " << ep.port() << endl;
}
//同步的tcp通信--server服务器端
//static表示只在本文件可见，否则出现重复定义
static void start_server()
{
	try
	{
		typedef ip::tcp::acceptor acceptor_type;
		typedef ip::tcp::endpoint endpoint_type;
		typedef ip::tcp::socket socket_type;

		cout << "server start." << endl;
		io_service io;
		//acceptor类封装了open+bind+listen等操作
		acceptor_type acceptor(io, endpoint_type(ip::tcp::v4(), 6688));
		cout << acceptor.local_endpoint().address() << endl;

		for (;;)
		{
			socket_type sock(io);
			acceptor.accept(sock); //接受链接
			cout << "client:";
			cout << sock.remote_endpoint().address() << endl;
			sock.send(buffer("hello asio"));
		}
	} catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
}
//同步的tcp通信--clinet客户端实现
//static表示只在本文件可见，否则出现重复定义
static void start_client()
{
	try
	{
		typedef ip::tcp::endpoint endpoint_type;
		typedef ip::tcp::socket socket_type;
		typedef ip::address address_type;

		cout << "client start." << endl;

		io_service io;

		socket_type sock(io);
		endpoint_type ep(address_type::from_string("127.0.0.1"), 6688);

		sock.connect(ep); //发起连接
		cout << sock.available() << endl;

		//vector<char> str(sock.available() + 1, 0);
		//sock.receive(buffer(str));
		//cout << "recive from " << sock.remote_endpoint().address();
		//cout << &str[0] << endl;

		vector<char> str(5, 0);
		error_code ec;
		for (;;)
		{
			sock.read_some(buffer(str), ec);
			if (ec)
			{
				break;
			}
			cout << &str[0];
		}
		cout << endl;
	} catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
}

#if 0
int main()
{
	case2();
}

#endif
