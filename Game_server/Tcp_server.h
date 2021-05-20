#pragma once
#include "Tcp_connection.h"

class Tcp_server :public std::enable_shared_from_this<Tcp_server>
{
public:
	Tcp_server(io_context _io, ip::tcp::endpoint _ep);
private:
	void accept_handler(std::shared_ptr<ip::tcp::socket>sock, const boost::system::error_code& ec);
	io_context& io;
	ip::tcp::endpoint ep;
	ip::tcp::acceptor acp;
};

