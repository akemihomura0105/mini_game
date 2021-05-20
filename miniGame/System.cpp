#include "System.h"
#include "tools.h"
#include <array>

int System::login()
{
	OTP("请输入用户名");
	std::string username;
	std::cin >> username;
	Proto_msg msg(1, 1);
	serialize(username, msg.body);
	auto buf = msg.encode();
	boost::system::error_code ec;
	sock->write_some(buffer(buf), ec);
	if (ec)
		std::cout << ec.message();
	else
	{
		std::array<uint8_t, 1>result_buf;
		read(*sock, buffer(buffer(result_buf), 1));
		if (ec)
			std::cout << ec.message();
		else
			return result_buf[0];
	}
}

void System::run()
{
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
