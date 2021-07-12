#include "Room_server.h"

Room_system::Room_system(io_context& _io, ip::tcp::endpoint& _room_ep) :io(_io), room_ep(_room_ep), acp(io, room_ep)
{
}

void Room_system::run(yield_context yield)
{
	auto ptr = shared_from_this();
	std::string system_ip = "127.0.0.1";
	std::string system_port = "12345";
	boost::system::error_code ec;
	ip::tcp::endpoint ep(ip::address::from_string(system_ip), stoi(system_port));
	auto sock = std::make_shared<ip::tcp::socket>(io);
	sock->async_connect(ep, yield[ec]);
	if (ec)
	{
		std::cerr << ec.message();
		return;
	}
	conn = std::make_shared<Tcp_connection>(io, sock, msg_que, 0);
	conn->run();
	auto msg = std::make_shared<Proto_msg>(1, 1000);
	constexpr int server_id = 0;
	serialize_obj(msg->body, server_id);
	conn->push_event(msg);
	spawn(io, bind(&Room_system::accept_handler, shared_from_this(), boost::placeholders::_1));
	io.post(bind(&Room_system::route, shared_from_this()));
}

void Room_system::accept_handler(yield_context yield)
{
	while (true)
	{
		boost::system::error_code ec;
		auto sock = std::make_shared<ip::tcp::socket>(io);
		acp.async_accept(*sock, yield[ec]);
		if (ec)
		{
			std::cerr << ec.message();
			sock->close();
			continue;
		}
		auto conn = std::make_shared<Tcp_connection>(io, sock, msg_que, -1);
		conn->run();
	}
}

void Room_system::verify_request(std::shared_ptr<Proto_msg>msg)
{
	int session_id, room_id, verify_code;
	deserialize_obj(msg->body, session_id, room_id, verify_code);

	// do some verify work.
	// ...
	// ...
	session[session_id] = Tcp_connection::get_conn_from_body(msg->body);
	room[room_id]->link_player(session_id, session[session_id]);
}

void Room_system::room_request(std::shared_ptr<Proto_msg> msg)
{
	Room_info room_info;
	deserialize_obj(msg->body, room_info);
	if (room.find(room_info.room_id) != room.end())
	{
		std::cerr << "receive a existed room" << std::endl;
		return;
	}
	auto new_room = std::make_shared<Game_room>(io, room_info);
	new_room->run();
	room[room_info.room_id] = new_room;
	auto ack_msg = std::make_shared<Proto_msg>(1, 502);
	serialize_obj(ack_msg->body, room_info.room_id);
	conn->push_event(ack_msg);
}

void Room_system::route()
{
	if (msg_que.empty())
	{
		io.post(bind(&Room_system::route, shared_from_this()));
		return;
	}
	std::shared_ptr<Proto_msg> msg = msg_que.front();
	msg_que.pop();
	switch (msg->head.service)
	{
	case 1001:
		verify_request(msg);
		break;
	case 501:
		room_request(msg);
		break;
	}
	io.post(bind(&Room_system::route, shared_from_this()));
}