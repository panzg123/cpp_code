/*
 * boost_asio_file_descriptor.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 *      Description :asio 操作文件描述符demo，此处是从标准输入流中异步读取数据
 */

#include "std.hpp"
using namespace std;

#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

using namespace boost::asio;
using namespace boost::system;

//////////////////////////////////////////

#if 0
int main()
{
    io_service io;

    //boost::asio::posix::stream_descriptor in(io, ::dup(STDIN_FILENO));
    typedef boost::asio::posix::stream_descriptor descriptor_type;
    descriptor_type in(io, STDIN_FILENO);                                                    //文件描述符
    vector<char> buf(30);                                                                                     //缓冲区

    //typedef void(handler_type)(const error_code&, std::size_t);

    //使用回调函数的方法来异步读取
    //function<handler_type> handler =
    //    [&](const error_code& ec, std::size_t len){
    //        if(ec)
    //        {return;}
    //        //cout << "get some data" << endl;
    //        if(len < buf.size())
    //        {
    //            buf[len] = 0;
    //        }
    //        cout << buf.data();
    //        in.async_read_some(buffer(buf), handler);
    //        };

    //in.async_read_some(buffer(buf), handler);


    //下面是使用协程的方法来异步读取
    spawn(io,                                          //产生协程
        [&](yield_context yield)           //lambda表达式
        {
            for(;;)
            {
                error_code ec;
                auto len = in.async_read_some(buffer(buf), yield[ec]);

                if(ec)
                {return;}

                if(len < buf.size())
                {
                    buf[len] = 0;
                }

                cout << buf.data();
            }
        }
    );

    io.run(); //启动时间循环
}

#endif
