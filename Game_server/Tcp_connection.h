#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>

using namespace boost::asio;

class Tcp_connection
{
public:
	Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket>_sock);
	void run();
private:
	io_context& io;
	std::shared_ptr<ip::tcp::socket>sock;
};