#pragma once
#include "Game_room.h"

class Room_system :public std::enable_shared_from_this<Room_system>
{
public:
	Room_system(io_context& io, ip::tcp::endpoint& room_ep);
	void run(yield_context yield);
private:
	//used for the verification, client should use the keyword from the system to get the authorization.
	//Hasn't been realized.
	std::unordered_map<int, int>conn_verification_info;

	io_context& io;
	ip::tcp::endpoint room_ep;
	ip::tcp::acceptor acp;
	std::shared_ptr<Tcp_connection> conn;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	std::unordered_map<int, std::shared_ptr<Tcp_connection>>session;
	std::unordered_map<int, std::shared_ptr<Game_room>>room;
	void accept_handler(yield_context yield);

	void verify_request(std::shared_ptr<Proto_msg>msg);
	void room_request(std::shared_ptr<Proto_msg> msg);

	void route();
};

