#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <memory>
#include "../proto/Game_proto.h"
#include "../state_code/state_code.h"
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
	void verify_login(boost::system::error_code& ec);
	void show_room();
	void join_room();
	void create_room();
	void get_ready();
};
