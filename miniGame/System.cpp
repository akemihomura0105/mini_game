#include "System.h"
#include "tools.h"
#include <array>

int System::login()
{
	OTP("请输入用户名");
	std::string username;
	std::cin >> username;
	Proto_msg msg(1, 1);
	serialize_obj(username, msg.body);
	auto buf = msg.encode();
	boost::system::error_code ec;
	write(*sock, buffer(*buf), ec);
	if (ec)
		std::cout << ec.message();
	else
	{
		auto reply_msg_ptr = get_msg();
		state_code sc;
		deserialize_obj(reply_msg_ptr->body, sc);
		if (sc == CODE::LOGIN_SUCCESS)
		{
			std::cout << "登录成功\n";
			return 0;
		}

		if (sc == CODE::LOGIN_REPEATED)
		{
			std::cout << "你已经在其他地点登录\n";
			return 1;
		}
	}
}

std::shared_ptr<Proto_msg> System::get_msg()
{
	auto msg_ptr = std::make_shared<Proto_msg>(1);
	boost::system::error_code ec;
	std::string s;
	s.resize(10000);
	read(*sock, buffer(s));
	read(*sock, buffer(&msg_ptr->head, sizeof(Proto_head)), ec);
	if (ec)
		std::cerr << ec.message() << std::endl;
	std::cerr << msg_ptr->head.service;
	std::cerr << msg_ptr->head.len << std::endl;
	msg_ptr->body.resize(msg_ptr->head.len);
	std::cerr << msg_ptr->head.len << std::endl;
	read(*sock, buffer(msg_ptr->body), ec);
	if (ec)
		std::cerr << ec.message() << std::endl;
	return msg_ptr;
}

void System::run()
{
	sock->connect(ep);
	while (login())
	{
		//do something
		login();
	}
	std::cout << "login successfully" << std::endl;

}

System::System(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep)
{
	sock = std::make_shared<ip::tcp::socket>(io);
	//sock->async_connect(ep, boost::bind(login));
}