#pragma once
#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <memory>
#include "../general_class/Game_proto.h"
#include "../general_class/state_code.h"
#include "Otp_table.h"
//#include "../Game_server/Game_room.h"
#include "Room_info.h"
using namespace boost::asio;
class System
{
public:
	void run();
	System(io_context& _io, ip::tcp::endpoint& _ep);

private:
	io_context& io;
	ip::tcp::endpoint& ep;
	std::shared_ptr<ip::tcp::socket>sock;
	size_t session_id;
	int login();
	std::shared_ptr<Proto_msg> get_msg();
	boost::system::error_code send_msg(Proto_msg msg);
	void verify_login(boost::system::error_code& ec);
	void show_room();
	void join_room();
	int make_room(int state);
	int exit_room(int state);
	void get_ready();
};
