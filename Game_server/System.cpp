#include "System.h"

ASYNC_RET System::login(std::shared_ptr<ip::tcp::socket>sock, const std::string& username, const std::string& password)
{
	User user;
	std::hash<std::string>hash;
	user.set_id(hash(username));
	auto login_msg = std::make_shared<Proto_msg>(1, 1);
	if (user_list.find(user) != user_list.end())
	{
		state_code sc;
		sc.set(CODE::LOGIN_REPEATED);
		serialize_obj(sc, login_msg->body);
		std::cout << "login failed\n";
	}
	else
	{
		user.load_user(username, password);
		user_list.insert(user);
		serialize_obj(CODE::LOGIN_SUCCESS, login_msg->body);
		std::cout << "login success\n";
	}
	post(bind(Tcp_connection::send_msg, sock, login_msg));
}

System& System::get_instance()
{
	static System system;
	return system;
}
