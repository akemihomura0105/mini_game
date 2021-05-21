#include "System.h"

using namespace boost::asio;

int main(int argc, char* argv[])
{
	io_context io;
	ip::tcp::endpoint ep(ip::address::from_string(argv[1]), atoi(argv[2]));
	System sys(io, ep);
	sys.run();
}