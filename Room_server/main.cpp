#include "Room_server.h"

int main()
{
	io_context io;
	ip::tcp::endpoint ep(ip::tcp::v4(), 10000);
	auto system = std::make_shared<Room_system>(io, ep);
	spawn(io, bind(&Room_system::run, system, boost::placeholders::_1));
	io.run();
}