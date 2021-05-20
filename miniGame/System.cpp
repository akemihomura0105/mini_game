#include "System.h"
#include "tools.h"

void System::login(boost::system::error_code& ec)
{
	OTP("请输入用户名");
	std::string username;
	std::cin >> username;
	//try_login();
}

System::System(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep)
{
	sock = std::make_shared<ip::tcp::socket>(io);
	//sock->async_connect(ep, boost::bind(login));
}
