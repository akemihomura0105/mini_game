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
		session_register[*session_id] = session_id;
		conn = std::make_shared<Tcp_connection>(io, sock, msg_que, *session_id);
		if (session.size() <= *session_id)
			session.push_back(conn);
		else
			session[*session_id] = conn;

		io.post(bind(&Tcp_connection::run, conn));
		auto msg = std::make_shared<Proto_msg>(1, 1);
		serialize_obj(msg->body, size_t(*session_id));
		conn->send_msg(msg);
	}
	std::cerr << "成功运行accept_handle\n";
	auto new_sock = std::make_shared<ip::tcp::socket>(io);
	acp.async_accept(*new_sock, boost::bind(&System::accept_handler, shared_from_this(), new_sock, placeholders::error));
}

ASYNC_RET System::login(std::shared_ptr<Proto_msg>msg)
{
	size_t session_id;
	std::string username, password;
	deserialize_obj(msg->body, session_id, username);
	auto user = std::make_shared<User>();
	auto login_msg = std::make_shared<Proto_msg>(1, 1);
	state_code sc;
	if (username_to_session.find(username) != username_to_session.end())
	{
		sc.set(CODE::LOGIN_REPEATED);
		serialize_obj(login_msg->body, sc);
		std::cout << "login failed\n";
	}
	else
	{
		user->load_user(username, password);
		username_to_session[username] = session_id;
		while (session_to_user.size() <= session_id)
			session_to_user.push_back(std::shared_ptr<User>());
		session_to_user[session_id] = user;
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
	auto res_msg = std::make_shared<Proto_msg>(1, 2);
	size_t session_id;
	deserialize_obj(msg->body, session_id);
	std::vector<Game_room::Room_property>room_vec;
	for (const auto& n : room)
		if (n.get_id() != 0)
			room_vec.emplace_back(n.get_Room_property());
	serialize_obj(res_msg->body, state_code(), room_vec);
	std::cerr << res_msg->body << std::endl;
	session[session_id]->send_msg(res_msg);
	return ASYNC_RET();
}

void System::create_room(std::shared_ptr<Proto_msg> msg)
{
	auto res_msg = std::make_shared<Proto_msg>(1, 3);
	size_t session_id;
	Game_room::Room_property room_property(true);
	deserialize_obj(msg->body, session_id, room_property.name, room_property.capacity);
	room_property.size = 1;
	std::cerr << "receive a room msg from " << session_id << ", room name is: " << room_property.name << ", room capacity is " << room_property.capacity << "\n";
	Game_room game_room(std::move(room_property));
	size_t room_id = game_room.get_id();
	while (room_id >= room.size())
		room.push_back(Game_room());
	room[room_id] = game_room;

	room[room_id].add_user(session_id);
	serialize_obj(res_msg->body, room_id);
	session[session_id]->push_event(res_msg);
	broadcast_room_info(room_id);
}

void System::join_room(std::shared_ptr<Proto_msg> msg)
{
	size_t session_id;
	size_t room_id;
	deserialize_obj(msg->body, session_id, room_id);
	state_code sc;
	auto res_msg = std::make_shared<Proto_msg>(1, 5);
	if (room_id >= room.size() || room[room_id].get_id() == 0)
		sc.set(CODE::ROOM_NOT_EXIST);
	else
		if (room[room_id].add_user(session_id) == 1)
			sc.set(CODE::ROOM_FULL);
	serialize_obj(res_msg->body, sc);
	session[session_id]->push_event(res_msg);
	broadcast_room_info(room_id);
}

void System::exit_room(std::shared_ptr<Proto_msg> msg)
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
	session[session_id]->push_event(res_msg);
	broadcast_room_info(room_id);
}

void System::broadcast_room_info(size_t room_id)
{
	Room_info info;
	const auto& prop = room[room_id].get_Room_property();
	info.capacity = prop.capacity;
	info.name = prop.name;
	info.room_id = room_id;
	const auto& user = room[room_id].get_Room_user();
	for (size_t n : user)
	{
		const auto& user = session_to_user[n];
		info.user.emplace_back(n, user->get_username(), user->is_ready());
	}
	auto msg = std::make_shared<Proto_msg>(1, 6);
	serialize_obj(msg->body, info);
	broadcast_event_in_room(room_id, msg);
}

void System::broadcast_event_in_room(size_t room_id, std::shared_ptr<Proto_msg> msg)
{
	const auto& users = room[room_id].get_Room_user();
	for (const auto& n : users)
		session[n]->push_event(msg);
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
		//case 1:登录服务
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