#include "Tcp_server.h"

void Tcp_server::accept_handler(std::shared_ptr<ip::tcp::socket> sock, const boost::system::error_code& ec)
{
	if (sock)
	{
		auto connection = std::make_shared<Tcp_connection>(io, sock);
		io.post(boost::bind(&Tcp_connection::run, connection));
	}
	auto new_sock = std::make_shared<ip::tcp::socket>(io);
	acp.async_accept(*new_sock, boost::bind(&Tcp_server::accept_handler, shared_from_this(), new_sock, placeholders::error));
}
