#pragma once
#include "User.h"
#include "Game_room.h"
#include "predefine.h"
#include "ID_generator.h"
#include "../general_class/Game_proto.h"
#include "Tcp_connection.h"
#include "../general_class/state_code.h"
#include <random>
#include <boost/asio.hpp>
#include <string>
#include <set>
#include <queue>
#include <boost/serialization/vector.hpp>

using namespace boost::asio;

class System :public std::enable_shared_from_this<System>
{
public:
	void run();
	ASYNC_RET accept_handler(std::shared_ptr<ip::tcp::socket>sock, const boost::system::error_code& ec);

	ASYNC_RET route();
	System(io_context& _io, ip::tcp::endpoint& _ep);
private:
	io_context& io;
	ip::tcp::endpoint& ep;
	ip::tcp::acceptor acp;

	ID_generator<size_t>session_gen;
	std::vector<std::shared_ptr<Tcp_connection>>session;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	std::unordered_map<User, size_t>user_to_session;//key:session_id, value:User
	std::unordered_map<size_t, User>session_to_user;

	ASYNC_RET login(std::shared_ptr<Proto_msg>msg);

	ASYNC_RET show_room(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET create_room(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET join_room(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET quit_room(std::shared_ptr<Proto_msg>msg);
	std::set<Game_room>room;
};