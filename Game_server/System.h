#pragma once
#include "User.h"
#include "Game_room.h"
#include "ID_generator.h"
#include "../general_class/Game_proto.h"
#include "../general_class/Tcp_connection.h"
#include "../general_class/state_code.h"
#include "../general_class/Room_info.h"
#include <random>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <string>
#include <set>
#include <queue>
#include <boost/serialization/vector.hpp>

using namespace boost::asio;

class System :public std::enable_shared_from_this<System>
{
public:
	void run();
	void close(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET accept_handler(std::shared_ptr<ip::tcp::socket>sock, const boost::system::error_code& ec);

	ASYNC_RET route();
	System(io_context& _io, ip::tcp::endpoint& _ep);
private:
	io_context& io;
	ip::tcp::endpoint& ep;
	ip::tcp::acceptor acp;

	ID_generator<int>session_gen;
	std::map <int, std::shared_ptr<ID<int>>>session_register;
	std::vector<std::shared_ptr<Tcp_connection>>session;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	std::unordered_map<std::string, int>username_to_session;//key:session_id, value:User
	std::vector<std::shared_ptr<User>>session_to_user;

	ASYNC_RET login(std::shared_ptr<Proto_msg>msg);

	ASYNC_RET show_room(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET create_room(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET join_room(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET exit_room(std::shared_ptr<Proto_msg>msg);

	void set_ready(std::shared_ptr<Proto_msg>msg);
	void start_game(std::shared_ptr<Proto_msg>msg);
	void listen_room(int room_id);

	void move_location(std::shared_ptr<Proto_msg>msg);
	void attack(std::shared_ptr<Proto_msg>msg);
	void heal(std::shared_ptr<Proto_msg>msg);
	void mine(std::shared_ptr<Proto_msg>msg);




	void broadcast_room_info(int room_id);
	void broadcast_event_in_room(int room_id, std::shared_ptr<Proto_msg>msg);
	void delete_room(int room_id);
	std::vector<std::shared_ptr<Game_room>>room;
};