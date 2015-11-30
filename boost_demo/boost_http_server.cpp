/*
 * boost_http_server.cpp
 *
 *  Created on: 2015年11月30日
 *      Author: panzg
 */
#include "std.hpp"
using namespace std;

#define BOOST_ASIO_DISABLE_STD_CHRONO
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include<boost/algorithm/string.hpp>
#include<sys/sendfile.h>
using namespace boost;
using namespace boost::asio;
using namespace boost::system;
//////////////////////////////////////////

class http_server
{
	typedef http_server this_type;
	typedef ip::tcp::acceptor acceptor_type;
	typedef ip::tcp::endpoint endpoint_type;
	typedef ip::tcp::socket socket_type;
	typedef boost::shared_ptr<socket_type> sock_ptr;
	typedef boost::shared_ptr<vector<char>> m_buf_ptr;

private:
	string str_html =
			"<html>\n<head>\n<title>Hello</title>\n</head>\n<body>\n<p>Hello world!!!</p>\n</body>\n</html>";
	io_service m_io;
	acceptor_type m_acceptor;
	vector<char> m_buf;
public:
	//构造函数
	http_server() :
			m_acceptor(m_io, endpoint_type(ip::tcp::v4(), 7008)),m_buf(1000,0)
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
//		sock->async_write_some(buffer(str_html),
//				bind(&this_type::write_handler2, this,
//						boost::asio::placeholders::error,
//						boost::asio::placeholders::bytes_transferred));

		//下面是接收消息
		sock->async_receive(buffer(m_buf),
				bind(&this_type::recv_handler, this,
						boost::asio::placeholders::error, sock));

		accept();
	}
	//接受连接的回调函数
	void recv_handler(const system::error_code &ec, sock_ptr sock)
	{
		cout<<"收到请求如下："<<endl;
		if(ec)
		return;
		//cout<<&m_buf[0]<<endl;
		//下面是请求资源进行解析
		string str = m_buf.data();
		vector<string> split_res;
		split(split_res,str,is_any_of(" "));
//		for(auto x : split_res)
//			cout<<"分割"<<x<<endl;
		if(split_res[0] !="GET")
			return;
		cout<<"method:"<<split_res[0]<<endl;
		//下面是读取文件信息进行返回
		string filename = split_res[1];
		string  pre_path = "htdocs";
		cout<<"filename"<<filename<<endl;
		string file_content = get_file_content(pre_path + filename);
		//下面是发送消息
		sock->async_send(buffer(file_content),
		bind(&this_type::write_handler2,this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
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
	//读取文件内容到字符串，返回字符串
	string get_file_content(string filename)
	{
		const char * name = filename.c_str();
		std::ifstream ifile(name);
		//将文件读入到ostringstream对象buf中
		ostringstream buf;
		char ch;
		while(buf&&ifile.get(ch))
		buf.put(ch);
		//返回与流对象buf关联的字符串
		return buf.str();
	}
};

int main()
{
	http_server svr;
	svr.run();
}

