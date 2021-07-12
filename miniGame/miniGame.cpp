#include "System.h"
#include <fstream>

using namespace boost::asio;

int main(int argc, char* argv[])
{
	std::ofstream fout("client.log");
	std::cerr.set_rdbuf(fout.rdbuf());
	io_context io;
	ip::tcp::endpoint ep(ip::address::from_string(argv[1]), atoi(argv[2]));
	auto sys = std::make_shared<System>(io, ep);
	sys->run();
}