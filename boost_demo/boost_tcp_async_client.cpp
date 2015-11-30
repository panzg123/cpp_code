/*
 * boost_tcp_async_client.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description : asio tcp异步 客户端demo
 */
#include"std.hpp"
using namespace std;
#define BOOST_ASIO_DISABLE_STD_CHRONO
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include<boost/smart_ptr.hpp>

using namespace boost::asio;
using namespace boost::system;

class client
{
	typedef client    this_type;
	typedef ip::tcp::endpoint endpoint_type;
	typedef ip::address             address_type;
	typedef ip::tcp::socket       socket_type;
	typedef boost::shared_ptr<socket_type> sock_ptr;
	typedef vector<char>  buffer_type;
private:
	io_service m_io;                                       //io_service对象
	endpoint_type m_ep;                                 //tcp端点
	buffer_type m_buf;                                 //接收缓冲区

public:
	//构造函数
	client():m_buf(100,0),m_ep(address_type::from_string("127.0.0.1"),6688)
{
		start();
}
	//run
	void run()
	{
		m_io.run();
	}
	//启动异步连接
	void start()
	{
		sock_ptr sock(new socket_type(m_io));  //创建socket对象
		sock->async_connect(m_ep,
				bind(&this_type::conn_handler,this,
						boost::asio::placeholders::error,sock));
	}
	//连接的回调函数
	void conn_handler(const error_code &ec,sock_ptr sock)
	{
		if(ec)
			return;
		cout<<"recive from "<<sock->remote_endpoint().address();
		sock->async_read_some(buffer(m_buf),
				bind(&this_type::read_handler,this,
						boost::asio::placeholders::error));
	}
	//接收数据的回调函数
	void read_handler(const error_code &ec)
	{
		if(ec)
			return;
		//这里为什么不直接cout<<m_buf<<endl
		cout<<&m_buf[0]<<endl;
	}
};
//初始化
static void init()
{
	try
	{
		cout<<"client start:"<<endl;
		client m_client;
		m_client.run();
	}
	catch (std::exception &e)
	{
		cout<<e.what()<<endl;
	}
}


