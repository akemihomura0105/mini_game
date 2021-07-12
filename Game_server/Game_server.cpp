#include "System.h"

int main()
{
	std::cerr << "boost version:" << BOOST_VERSION << std::endl;
	//std::ifstream config_file("config.zf");

	io_context io;
	ip::tcp::endpoint system_server_ep(ip::tcp::v4(), 12345);
	std::vector<ip::tcp::endpoint> room_server_ep;

	auto system = std::make_shared<System>(io, system_server_ep);
	system->run();
	io.run();
}