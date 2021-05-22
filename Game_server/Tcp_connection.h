#pragma once
#include"../proto/Game_proto.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <queue>
#include <iostream>
//#include "Router.h"
#include "System.h"

using namespace boost::asio;

class Tcp_connection :public std::enable_shared_from_this<Tcp_connection>
{
public:
	Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket>_sock);
	void run();
	void get_msg_head();
	void get_msg_body(std::shared_ptr<Proto_msg>proto_ptr, const boost::system::error_code& ec);
	void push_msg(std::shared_ptr<Proto_msg>proto_ptr, const boost::system::error_code& ec);

	void push_event(std::shared_ptr<Proto_msg>msg_ptr);
	void send_event();
	static ASYNC_RET send_msg(std::shared_ptr<ip::tcp::socket>sock, std::shared_ptr<Proto_msg>msg_ptr);
	void route(std::shared_ptr<Proto_msg>msg_ptr);
	static void socket_error_handle(std::shared_ptr<std::string>buf, const boost::system::error_code& ec);
private:
	io_context& io;
	std::shared_ptr<ip::tcp::socket>sock;
	std::queue<std::shared_ptr<Proto_msg>>event_que;
};