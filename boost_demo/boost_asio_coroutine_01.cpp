/*
 * boost_asio_coroutine_01.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description : 使用asio协程的异步服务器demo，协程需要链接libboost_context.a , libboost_coroutine.a , libboost_thread.a 三个库
 */

#include "std.hpp"
using namespace std;

#define BOOST_ASIO_DISABLE_STD_CHRONO
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp> //协程头文件

using namespace boost::asio;
using namespace boost::system;

//////////////////////////////////////////
static void init()
{
	typedef ip::tcp::acceptor acceptor_type;
	typedef ip::tcp::endpoint endpoint_type;
	typedef ip::tcp::socket socket_type;

	io_service io;

	spawn(io, [&](yield_context yield)          //spawn函数产生协程，使用lambda表达式
	{
		acceptor_type acceptor(io,                   //acceptor对象
				endpoint_type(ip::tcp::v4(), 6688));

		for(;;)                                                            //开始循环侦听
		{
			socket_type sock(io);
			error_code ec;

			acceptor.async_accept(sock, yield[ec]);  //使用协程，无handler

			if(ec) //检查错误码
			{
				return;
			}

			auto len = sock.async_write_some( //异步写数据
					buffer("hello coroutine"), yield);          //使用协程
			cout << "send " << len << " bytes" << endl;
		}
	});

	io.run();//启动事件循环
}

#if 0
int main()
{
	init();
}
#endif
