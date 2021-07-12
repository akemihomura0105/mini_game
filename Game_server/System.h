#pragma once
#include<boost/asio/spawn.hpp>
#include "User.h"
#include "Game_room.h"
#include "ID_generator.h"
#include "../general_class/Game_proto.h"
#include "../general_class/Tcp_connection.h"
#include "../general_class/state_code.h"
#include "../general_class/Room_info.h"
#include <fstream>
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
	void accept_handler(yield_context yield);

	void route();
	System(io_context& _io, ip::tcp::endpoint& system__server_ep);
private:
	io_context& io;
	ip::tcp::endpoint system_server_ep;
	std::unordered_map<int, std::shared_ptr<Tcp_connection>>room_server;
	ip::tcp::acceptor acp;

	ID_generator<int>session_gen;
	std::map <int, std::shared_ptr<ID<int>>>session_register;
	std::vector<std::shared_ptr<Tcp_connection>>session;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	std::unordered_map<std::string, int>username_to_session;//key:session_id, value:User
	std::vector<std::shared_ptr<User>>session_to_user;

	void regist_room_server(std::shared_ptr<Proto_msg>msg);
	void login(std::shared_ptr<Proto_msg>msg);

	void show_room(std::shared_ptr<Proto_msg>msg);
	void create_room(std::shared_ptr<Proto_msg>msg);
	void join_room(std::shared_ptr<Proto_msg>msg);
	void exit_room(int session_id, int room_id);
	void exit_room(std::shared_ptr<Proto_msg>msg);

	void set_ready(std::shared_ptr<Proto_msg>msg);
	void start_game(std::shared_ptr<Proto_msg>msg);
	void receive_room_server_start_ack(std::shared_ptr<Proto_msg> msg);
	void send_room_msg_to_client(int room_id);

	const Room_info get_room_info(int room_id)const;
	void broadcast_room_info(int room_id);
	void broadcast_event_in_room(int room_id, std::shared_ptr<Proto_msg>msg);
	void delete_room(int room_id);
	std::vector<std::shared_ptr<Game_room>>room;
};