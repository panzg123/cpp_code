/*
 * boost_asio_udp_01.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description :asio udp通信
 */
#include "std.hpp"
using namespace std;

#define BOOST_ASIO_DISABLE_STD_CHRONO
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
//#include <boost/asio/use_future.hpp>
using namespace boost::asio;
using namespace boost::system;
//asio udp 客户端demo
static void udp_client()
{
	cout << "client start." << endl;
	io_service io;

	ip::udp::endpoint send_ep(ip::address::from_string("127.0.0.1"), 6699);
	ip::udp::socket sock(io);
	sock.open(ip::udp::v4());

	char buf[1];
	sock.send_to(buffer(buf), send_ep);

	vector<char> v(100, 0);
	ip::udp::endpoint recv_ep;
	sock.receive_from(buffer(v), recv_ep);
	cout << "recv from " << recv_ep.address() << " ";
	cout << &v[0] << endl;
}
//asio udp 服务器demo
static void udp_server()
{
	cout << "udp server start." << endl;
	io_service io;

	ip::udp::socket sock(io, ip::udp::endpoint(ip::udp::v4(), 6699));

	for (;;)
	{
		char buf[1];
		ip::udp::endpoint ep;

		error_code ec;
		sock.receive_from(buffer(buf), ep, 0, ec);

		if (ec && ec != error::message_size)
		{
			throw system_error(ec);
		}

		cout << "send to " << ep.address() << endl;
		sock.send_to(buffer("hello asio udp"), ep);
	}
}

