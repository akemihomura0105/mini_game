#pragma once
#include"../general_class/Game_proto.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>
#include <queue>
#include <iostream>

#define ASYNC_RET void

using namespace boost::asio;

//
// Fundermental service class.
// Provide tcp socket encapsulation.
// Provide a read/write buffer of 6
// 36 bytes in size.
// 
// Provides async parsing of the @obj Proto_msg.
// After calling the run function, the program will execute the following function:
//	 @fun get_msg_head()
//		parasing the Proto_head
//	 @fun get_msg_body()
//		use the head.len to parase the body;
//   @fun push_msg()
//		the parsed packets will be sent to @obj msg_que 
//		which has been binded in the construct function.
// 
// Provides serialized output to sockets.
// The sent messages will be saved in the event queue. 
// Use the @fun push_event to push a @obj Proto_msg, 
// then the async function @fun send_event will send the message to socket. 
//

class Tcp_connection :public std::enable_shared_from_this<Tcp_connection>
{
public:
	//Binding a io_context, a boost::ip::tcp::socket, 
	//the message queue used for receiving packets,
	//and the session id used for identifying this connection.
	Tcp_connection(io_context& io, std::shared_ptr<ip::tcp::socket>sock,
		std::queue<std::shared_ptr<Proto_msg>>& msg_que, int session_id);
	//Run the connection. Functions related to read and write will be async runned.
	void run();
	//Not realization
	void pause();
	//close the tcp connection, not fully realization.
	void close();
	void get_msg(yield_context yield);
	void get_msg_head();
	void get_msg_body(std::shared_ptr<Proto_msg>proto_ptr, const boost::system::error_code& ec);

	void push_event(std::shared_ptr<Proto_msg>msg_ptr);
	bool socket_error_solve(const boost::system::error_code& ec);
	void socket_error_handle(const boost::system::error_code& ec);
	~Tcp_connection() { sock->close(); }
private:
	io_context& io;
	std::shared_ptr<ip::tcp::socket>sock;
	std::queue<std::shared_ptr<Proto_msg>>event_que;
	std::queue<std::shared_ptr<Proto_msg>>& msg_que;
	bool pause_flag = false;
	void send_event();
	void push_msg(std::shared_ptr<Proto_msg>proto_ptr, const boost::system::error_code& ec);
	std::string write_buf, read_buf;
	int session_id;
};