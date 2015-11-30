/*
 * signal_demo_01.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description : asio 信号 demo
 */
#include "std.hpp"
using namespace std;

#define BOOST_ASIO_DISABLE_STD_CHRONO
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/function.hpp>
#include <boost/asio.hpp>
using namespace boost::asio;
using namespace boost::system;


#define BOOST_ASIO_DISABLE_STD_CHRONO
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING           //启用跟踪日志
#include <boost/function.hpp>
#include <boost/asio.hpp>
using namespace boost::asio;
using namespace boost::system;

// kill -10 xxxx

//出现段错误
#if 0
int main()
{
    io_service io;
    //io_service::work w(io);

    signal_set sig(io, SIGINT, SIGUSR1);
    cout << "add:" << SIGINT << "," << SIGUSR1 << endl;

    //sig.add(SIGINT);
    //sig.add(SIGUSR1);

    auto handler1 =
        [&](const error_code& ec, int n)
        {
            //cout << "enter sigint" << endl;
            if(ec)
            {
                cout << ec.message() << endl;
                return;
            }

            if(n != SIGINT)
            {
                return;
            }

            cout << "handler1 recv = " << n << endl;
            cout << "do something" << endl;
            //w.~work();
        };

    typedef void(handler_type)(const error_code&, int);
    function<handler_type>
        handler2 =
        [&](const error_code& ec, int n)
        {
            if(n != SIGUSR1)
            {
                return;
            }

            cout << "handler2 recv = " << n << endl;

            //让程序继续捕捉信号
            sig.async_wait(handler1);
            sig.async_wait(handler2);
        };

    sig.async_wait(handler1);
    sig.async_wait(handler2);

    io.run();
    cout << "io stoped" << endl;
}
#endif
