#include "System.h"

int main()
{
	io_context io;
	ip::tcp::endpoint ep(ip::tcp::v4(), 12345);
	auto system = std::make_shared<System>(io, ep);
	system->run();
	io.run();
}