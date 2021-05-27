#pragma once
#include"../general_class/Game_proto.h"
#include "ID_generator.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <queue>
#include <iostream>
#include "predefine.h"
//#include "Router.h"

using namespace boost::asio;

class Tcp_connection :public std::enable_shared_from_this<Tcp_connection>
{
public:
	Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket>_sock, std::queue<std::shared_ptr<Proto_msg>>& msg_que, std::shared_ptr<ID<size_t>> session_id);
	void run();

	void get_msg_head();
	void get_msg_body(std::shared_ptr<Proto_msg>proto_ptr, const boost::system::error_code& ec);
	void push_msg(std::shared_ptr<Proto_msg>proto_ptr, const boost::system::error_code& ec);

	void push_event(std::shared_ptr<Proto_msg>msg_ptr);
	ASYNC_RET send_event(const boost::system::error_code& ec);
	ASYNC_RET send_msg(std::shared_ptr<Proto_msg>msg_ptr);
	ASYNC_RET socket_error_handle(const boost::system::error_code& ec);
private:
	io_context& io;
	std::shared_ptr<ip::tcp::socket>sock;
	std::queue<std::shared_ptr<Proto_msg>>event_que;
	std::queue<std::shared_ptr<Proto_msg>>& msg_que;
	std::string write_buf, read_buf;
	std::shared_ptr<ID<size_t>>session_id;
};