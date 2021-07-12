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
// Provide asio tcp socket encapsulation.
// Provide a read/write buffer of 65536 bytes in size.
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
// When the service_id is [1000,1999], the connection class should push itself in the end of the message body.
// The connection would pass in the form of a void* pointer which reinterpret as a std::shared_ptr<Tcp_connection>* pointer.
// You must use the @static_fun get_conn_from_body to get the connection and this function will free the space automatically
//

class Tcp_connection :public std::enable_shared_from_this<Tcp_connection>
{
public:
	static std::shared_ptr<Tcp_connection> get_conn_from_body(std::string& body);
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

	void push_event(std::shared_ptr<Proto_msg>msg_ptr);
	bool socket_error_solve(const boost::system::error_code& ec);

	bool set_msg_que(std::queue<std::shared_ptr<Proto_msg>>& new_msg_que);
	~Tcp_connection() { sock->close(); }
private:
	io_context& io;
	std::shared_ptr<ip::tcp::socket>sock;
	std::queue<std::shared_ptr<Proto_msg>>event_que;

	typedef std::pair<std::shared_ptr<Proto_msg>, std::shared_ptr<Tcp_connection>>msg_pair;
	std::queue<std::shared_ptr<Proto_msg>>* msg_que;
	bool pause_flag = false;
	void send_event();
	std::string write_buf, read_buf;
	int session_id;
};