#include "System.h"

System::System(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep), acp(_io, _ep), session_gen(0, 10000)
{
}

void System::run()
{
	auto ec_ptr = std::make_shared<boost::system::error_code>();
	io.post(bind(&System::accept_handler, shared_from_this(), nullptr, *ec_ptr));
	io.post(bind(&System::route, shared_from_this()));
}

void System::accept_handler(std::shared_ptr<ip::tcp::socket>sock, const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message();
	}
	if (sock)
	{
		std::shared_ptr<Tcp_connection>conn;
		auto session_id = session_gen.generate();
		conn = std::make_shared<Tcp_connection>(io, sock, msg_que, session_id);
		if (session.size() <= *session_id)
			session.push_back(conn);
		else
			session[*session_id] = conn;

		io.post(bind(&Tcp_connection::run, conn));
		auto msg = std::make_shared<Proto_msg>(1, 1);
		serialize_obj(msg->body, size_t(*session_id));
		io.post(bind(&Tcp_connection::send_msg, conn, msg));
	}
	std::cerr << "�ɹ�����accept_handle\n";
	auto new_sock = std::make_shared<ip::tcp::socket>(io);
	acp.async_accept(*new_sock, boost::bind(&System::accept_handler, shared_from_this(), new_sock, placeholders::error));
}

ASYNC_RET System::login(std::shared_ptr<Proto_msg>msg)
{
	size_t session_id;
	std::string username, password;
	deserialize_obj(msg->body, session_id, username);
	User user;
	auto login_msg = std::make_shared<Proto_msg>(1, 1);
	state_code sc;
	if (user_to_session.find(user) != user_to_session.end())
	{
		sc.set(CODE::LOGIN_REPEATED);
		serialize_obj(login_msg->body, sc);
		std::cout << "login failed\n";
	}
	else
	{
		user.load_user(username, password);
		user_to_session[user] = session_id;
		sc.set(CODE::LOGIN_SUCCESS);
		serialize_obj(login_msg->body, sc);
		std::string str;
		serialize_obj(str, std::string("ss"));
		std::cout << "login success\n";
	}
	io.post(bind(&Tcp_connection::send_msg, session[session_id], login_msg));
}

ASYNC_RET System::show_room(std::shared_ptr<Proto_msg> msg)
{
	auto req_msg = std::make_shared<Proto_msg>(1, 2);
	size_t session_id;
	deserialize_obj(msg->body, session_id);
	std::vector<Game_room::Room_property>room_vec;
	for (const auto& n : room)
		room_vec.emplace_back(n.get_Room_property());
	serialize_obj(req_msg->body, state_code(), room_vec);
	session[session_id]->send_msg(req_msg);
	return ASYNC_RET();
}

ASYNC_RET System::create_room(std::shared_ptr<Proto_msg> msg)
{
	auto res_msg = std::make_shared<Proto_msg>(1, 3);
	size_t session_id;
	Game_room::Room_property room_property;
	deserialize_obj(msg->body, session_id, room_property.name, room_property.capacity);
	std::cerr << "receive a room msg from " << session_id << ", room name is: " << room_property.name << ", room capacity is " << room_property.capacity << "\n";
	Game_room game_room(std::move(room_property));
	size_t room_id = game_room.get_id();
	while (room_id >= room.size())
		room.push_back(Game_room());
	room[room_id] = game_room;

	room[room_id].add_user(session_id);
	serialize_obj(res_msg->body, room_id);
	session[session_id]->send_msg(res_msg);
}

ASYNC_RET System::join_room(std::shared_ptr<Proto_msg> msg)
{
	size_t session_id;
	size_t room_id;
	deserialize_obj(msg->body, session_id, room_id);
	state_code sc;
	if (room[room_id].add_user(session_id) == 1)
		sc.set(CODE::ROOM_FULL);
	auto res_msg = std::make_shared<Proto_msg>(1, 5);
	serialize_obj(res_msg->body, sc);
	session[session_id]->send_msg(res_msg);
}

ASYNC_RET System::exit_room(std::shared_ptr<Proto_msg> msg)
{
	size_t session_id;
	size_t room_id;
	deserialize_obj(msg->body, session_id, room_id);
	int ec = room[room_id].remove_user(session_id);
	switch (ec)
	{
	default:
		break;
	case 1:
		std::cerr << "user not found\n";
		break;
	case 2:
		delete_room(room_id);
	}
	auto res_msg = std::make_shared<Proto_msg>(1, 4);
	serialize_obj(res_msg->body, state_code());
	session[session_id]->send_msg(res_msg);
}

ASYNC_RET System::get_room_info(std::shared_ptr<Proto_msg> msg)
{
	size_t session_id;
	size_t room_id;
	deserialize_obj(msg->body, session_id, room_id);
	//auto prop = room[room_id].get_Room_property();
	//auto user = room[room_id].get_Room_user();
	room[room_id];
	auto res_msg = std::make_shared<Proto_msg>(1, 6);
	//serialize_obj(res_msg->body, prop, user);
	session[session_id]->push_event(res_msg);

}

void System::delete_room(size_t room_id)
{
	return;
}

ASYNC_RET System::route()
{
	if (msg_que.empty())
	{
		io.post(bind(&System::route, shared_from_this()));
		return;
	}
	std::shared_ptr<Proto_msg> msg = msg_que.front();
	msg_que.pop();
	std::cout << msg->head.service << std::endl;
	switch (msg->head.service)
	{
		//case 1:��¼����
	case 1:
	{
		login(msg);
		break;
	}
	case 2:
	{
		show_room(msg);
		break;
	}
	case 3:
	{
		create_room(msg);
		break;
	}
	case 4:
	{
		exit_room(msg);
		break;
	}
	case 5:
	{
		join_room(msg);
		break;
	}
	case 6:
	{

	}
	default:
		std::cerr << "undefined service packet\n";
	}
	io.post(bind(&System::route, shared_from_this()));
}