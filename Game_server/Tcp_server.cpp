#include "Tcp_server.h"

Tcp_server::Tcp_server(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep), acp(_io, _ep)
{

}

void Tcp_server::run()
{
	auto ec_ptr = std::make_shared<boost::system::error_code>();
	accept_handler(nullptr, *ec_ptr);
}

void Tcp_server::accept_handler(std::shared_ptr<ip::tcp::socket>sock, const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message();
	}
	if (sock)
	{
		std::cerr << "socket 接受\n";
		auto connection = std::make_shared<Tcp_connection>(io, sock);
		io.post(boost::bind(&Tcp_connection::run, connection));
	}
	std::cerr << "成功运行accept_handle\n";
	auto new_sock = std::make_shared<ip::tcp::socket>(io);
	acp.async_accept(*new_sock, boost::bind(&Tcp_server::accept_handler, shared_from_this(), new_sock, placeholders::error));
}