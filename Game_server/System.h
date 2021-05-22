#pragma once
#include "User.h"
#include "predefine.h"
#include "../proto/Game_proto.h"
#include "Tcp_connection.h"
#include "../state_code/state_code.h"
#include <boost/asio.hpp>
#include <string>
#include <set>

using namespace boost::asio;

class System
{
public:
	static System& get_instance();
	System(const System&) = delete;
	System operator=(const System&) = delete;
	ASYNC_RET login(std::shared_ptr<ip::tcp::socket>sock, const std::string& username, const std::string& password = "");
private:
	System() {};
	std::set<User>user_list;

};