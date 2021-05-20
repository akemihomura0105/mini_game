#include "Tcp_connection.h"

Tcp_connection::Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket> _sock) :io(_io), sock(_sock)
{

}

void Tcp_connection::run()
{
	
}
