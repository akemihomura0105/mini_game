#include "Tcp_server.h"

int main()
{
	io_context io;
	ip::tcp::endpoint ep(ip::tcp::v4(), 12345);
	auto server = std::make_shared<Tcp_server>(io, ep);
	server->run();
	io.run();
}