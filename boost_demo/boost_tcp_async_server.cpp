/*
 * boost_tcp_async_server.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description : asio tcp异步 服务器demo
 */

#include "std.hpp"
using namespace std;

#define BOOST_ASIO_DISABLE_STD_CHRONO
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::system;
//////////////////////////////////////////

class server
{
	typedef server this_type;
	typedef ip::tcp::acceptor acceptor_type;
	typedef ip::tcp::endpoint endpoint_type;
	typedef ip::tcp::socket socket_type;
	typedef boost::shared_ptr<socket_type> sock_ptr;

private:
	io_service m_io;
	acceptor_type m_acceptor;
public:
	//构造函数
	server() :
			m_acceptor(m_io, endpoint_type(ip::tcp::v4(), 6688))
	{
		accept();  //启动监听
	}
	void run()
	{
		m_io.run();
	}
	void accept()
	{
		sock_ptr sock(new socket_type(m_io));
		m_acceptor.async_accept(*sock,
				bind(&this_type::accept_handler, this,
						boost::asio::placeholders::error, sock));
	}
	//接受连接的回调函数
	void accept_handler(const error_code& ec, sock_ptr sock)
	{
		if (ec)
		{
			return;
		}

		cout << "client:";
		cout << sock->remote_endpoint().address() << endl;
		sock->async_write_some(buffer("hello asio"),
				bind(&this_type::write_handler2, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));

		accept();
	}
	//发送数据的回调函数
	void write_handler(const error_code&)
	{
		cout << "send msg complete." << endl;
	}
	//带字节数的handler
	void write_handler2(const error_code&, std::size_t n)
	{
		cout << "send msg " << n << endl;
	}
};

#if 0
int main()
{
	cout<<"server start."<<endl;
	server srv;
	srv.run();
}
#endif
