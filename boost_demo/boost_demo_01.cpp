//============================================================================
// Name        : boost_demo_01.cpp
// Author      : panzg
// Version     :
// Copyright   : Your copyright notice
// Description : 同步定时器和异步定时器demo
//============================================================================

#define BOOST_ASIO_DISABLE_STD_CHRONO
#include<boost/asio/steady_timer.hpp>                /*steady_timer是basic_waitable_timer的模板特化typedef*/
#include <boost/asio.hpp>
#include <iostream>


using namespace boost::asio;    //asio需要依赖于boost.system
using namespace boost::system;
using namespace std;
/////////////////////////////////////
#include<boost/chrono.hpp>
using namespace boost::chrono; //chrono需要依赖于boost_chrono酷
//seconds operator""  _s(unsigned long long n)
//{
//    return seconds(n);
//}
//
//milliseconds operator"" _ms(unsigned long long n)
//{
//    return milliseconds(n);
//}
////////////////////////////////////
//同步定时器
static void case1()
{
	io_service io;
	steady_timer t(io, milliseconds(500));
	cout << t.expires_at() << endl;
	cout << t.expires_from_now() << endl;

	t.wait();
	cout << "hello asio1" << endl;
}
//异步定时器
static void case2()
{
	io_service io;
	steady_timer t(io,milliseconds(500));
	t.async_wait(
			[](const error_code& ec){
		cout<<"hello asio"<<endl;
	});
	io.run();
}

#if 0
int main()
{
	case2();
	return 0;
}
#endif
