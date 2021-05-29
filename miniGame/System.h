#pragma once
#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <memory>
#include <list>
#include "../general_class/Game_proto.h"
#include "../general_class/state_code.h"
#include "../general_class/User_info.h"
#include "Otp_table.h"
#include "../general_class/Tcp_connection.h"
#include "../general_class/Room_info.h"
#include <queue>

using namespace boost::asio;

#define ASYNC_RET void
class System :public std::enable_shared_from_this<System>
{
public:
	void run();
	System(io_context& _io, ip::tcp::endpoint& _ep);

private:
	io_context& io;
	ip::tcp::endpoint& ep;
	std::shared_ptr<ip::tcp::socket>sock;
	std::shared_ptr<Tcp_connection>conn;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	size_t session_id;
	size_t room_id;

	int login();
	std::shared_ptr<Proto_msg> get_msg();
	boost::system::error_code send_msg(Proto_msg msg);


	void show_room();
	int join_room(int state);
	int make_room(int state);
	int exit_room(int state);


	void room_system_run();
	void update_room_info(std::shared_ptr<Proto_msg>msg);
	ASYNC_RET route();
	void get_ready();
};
