#pragma once
#include "User.h"
#include "predefine.h"
#include "../proto/Game_proto.h"
#include "Tcp_connection.h"
#include "../state_code/state_code.h"
#include <random>
#include <boost/asio.hpp>
#include <string>
#include <set>
#include <queue>

using namespace boost::asio;

class System :public std::enable_shared_from_this<System>
{
public:
	void run();
	ASYNC_RET accept_handler(std::shared_ptr<ip::tcp::socket>sock, const boost::system::error_code& ec);

	ASYNC_RET login(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET route();
	System(io_context& _io, ip::tcp::endpoint& _ep);
private:
	io_context& io;
	ip::tcp::endpoint& ep;
	ip::tcp::acceptor acp;

	std::vector<std::shared_ptr<Tcp_connection>>session;
	std::priority_queue<size_t>session_gc;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;

	std::set<User>user_list;
};