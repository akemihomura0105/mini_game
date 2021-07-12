#include "System.h"

System::System(io_context& _io, ip::tcp::endpoint& _system_server_ep) :
	io(_io), system_server_ep(_system_server_ep), acp(_io, _system_server_ep), session_gen(0, 10000)
{
}

void System::run()
{
	static boost::system::error_code ec;
	auto self = shared_from_this();
	spawn(io, bind(&System::accept_handler, shared_from_this(), boost::placeholders::_1));
	io.post(bind(&System::route, shared_from_this()));
}

void System::close(std::shared_ptr<Proto_msg>msg)
{
	int session_id;
	deserialize_obj(msg->body, session_id);
	//release tcp connection
	session[session_id].reset();
	if (session_to_user.size() > session_id && session_to_user[session_id] != nullptr)
	{
		auto user = session_to_user[session_id];
		//release from room
		if (user->get_room_id())
			exit_room(session_id, user->get_room_id());
		username_to_session.erase(session_to_user[session_id]->get_username());
		//release from user
		session_to_user[session_id].reset();
	}
	//release from session
	session_register.erase(session_id);
}

void System::accept_handler(yield_context yield)
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

		//regist the session
		std::shared_ptr<Tcp_connection>conn;
		auto session_id = session_gen.generate();
		session_register[*session_id] = session_id;
		conn = std::make_shared<Tcp_connection>(io, sock, msg_que, *session_id);
		if (session.size() <= *session_id)
			session.push_back(conn);
		else
			session[*session_id] = conn;

		conn->run();
		auto msg = std::make_shared<Proto_msg>(1, 0);
		serialize_obj(msg->body, int(*session_id));
		conn->push_event(msg);
		std::cerr << "accept a socket connect successfully\n";
	}

}

void System::regist_room_server(std::shared_ptr<Proto_msg> msg)
{
	int server_id;
	deserialize_obj(msg->body, server_id);
	room_server[server_id] = Tcp_connection::get_conn_from_body(msg->body);
}

void System::login(std::shared_ptr<Proto_msg>msg)
{
	int session_id;
	std::string username, password;
	std::cerr << msg->body;
	deserialize_obj(msg->body, session_id, username);
	auto user = std::make_shared<User>();
	auto login_msg = std::make_shared<Proto_msg>(1, 52);
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
		std::cout << "login success\n";
	}
	session[session_id]->push_event(login_msg);
}

void System::show_room(std::shared_ptr<Proto_msg> msg)
{
	auto res_msg = std::make_shared<Proto_msg>(1, 2);
	int session_id;
	deserialize_obj(msg->body, session_id);
	std::vector<Game_room::Room_property>room_vec;
	for (const auto& n : room)
		if (n != nullptr)
			room_vec.emplace_back(n->get_Room_property());
	serialize_obj(res_msg->body, state_code(), room_vec);
	std::cerr << res_msg->body << std::endl;
	session[session_id]->push_event(res_msg);
}

void System::create_room(std::shared_ptr<Proto_msg> msg)
{
	auto res_msg = std::make_shared<Proto_msg>(1, 3);
	int session_id;
	Game_room::Room_property room_property(true);
	deserialize_obj(msg->body, session_id, room_property.name, room_property.capacity);
	std::cerr << "receive a room msg from " << session_id << ", room name is: " << room_property.name << ", room capacity is " << room_property.capacity << "\n";
	auto game_room = std::make_shared<Game_room>(std::move(room_property), &io);
	int room_id = game_room->get_id();
	while (room_id >= room.size())
		room.emplace_back(nullptr);
	room[room_id] = game_room;

	room[room_id]->add_user(session_id, session_to_user[session_id]);
	serialize_obj(res_msg->body, game_room->get_Room_property());
	session[session_id]->push_event(res_msg);
	broadcast_room_info(room_id);
}

void System::join_room(std::shared_ptr<Proto_msg> msg)
{
	int session_id;
	int room_id;
	deserialize_obj(msg->body, session_id, room_id);
	state_code sc;
	auto res_msg = std::make_shared<Proto_msg>(1, 5);
	if (room_id >= room.size() || room[room_id] == nullptr)
		sc.set(CODE::ROOM_NOT_EXIST);
	else
		if (room[room_id]->add_user(session_id, session_to_user[session_id]) == 1)
			sc.set(CODE::ROOM_FULL);
	serialize_obj(res_msg->body, sc, room_id);
	session[session_id]->push_event(res_msg);
	if (sc == CODE::NONE)
		broadcast_room_info(room_id);
}

void System::exit_room(int session_id, int room_id)
{
	int ec = room[room_id]->remove_user(session_id, session_to_user[session_id]);
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
}

void System::exit_room(std::shared_ptr<Proto_msg> msg)
{
	int session_id;
	int room_id;
	deserialize_obj(msg->body, session_id, room_id);
	exit_room(session_id, room_id);
}

void System::set_ready(std::shared_ptr<Proto_msg> msg)
{
	int session_id, room_id;
	deserialize_obj(msg->body, session_id, room_id);
	session_to_user[session_id]->set_ready();
	broadcast_room_info(room_id);
}

void System::start_game(std::shared_ptr<Proto_msg> msg)
{
	state_code sc;
	int session_id, room_id;
	deserialize_obj(msg->body, session_id, room_id);
	const auto& user = room[room_id]->get_Room_user();
	bool judge = false;
	if (!judge && user.front() != session_id)
	{
		sc.set(CODE::NOT_HOME_OWNER);
		judge = true;
	}
	if (!judge && user.size() != room[room_id]->get_Room_property().capacity)
	{
		sc.set(CODE::ROOM_NOT_FULL);
		judge = true;
	}
	if (!judge)
	{
		for (auto n : user)
		{
			if (!session_to_user[n]->is_ready())
			{
				sc.set(CODE::NOT_READY);
				judge = true;
			}
		}
	}
	if (!judge)
	{
		auto msg = std::make_shared<Proto_msg>(1, 501);
		serialize_obj(msg->body, get_room_info(room_id));
		room_server[0]->push_event(msg);		
		return;
	}
	auto res_msg = std::make_shared<Proto_msg>(1, 52);
	serialize_obj(res_msg->body, sc);
	if (judge)
		session[session_id]->push_event(res_msg);
}

void System::receive_room_server_start_ack(std::shared_ptr<Proto_msg> msg)
{
	int room_id;
	deserialize_obj(msg->body, room_id);
	std::string ip, port;
	ip = "127.0.0.1";
	port = "10000";
	auto res_msg = std::make_shared<Proto_msg>(1, 50);
	serialize_obj(res_msg->body, ip, port);
	broadcast_event_in_room(room_id, res_msg);
}

const Room_info System::get_room_info(int room_id)const
{
	Room_info info;
	const auto& prop = room[room_id]->get_Room_property();
	info.capacity = prop.capacity;
	info.name = prop.name;
	info.room_id = room_id;
	const auto& user = room[room_id]->get_Room_user();
	for (int n : user)
	{
		const auto& user = session_to_user[n];
		info.user.emplace_back(n, user->get_username(), user->is_ready());
	}
	return info;
}

void System::broadcast_room_info(int room_id)
{
	Room_info info = get_room_info(room_id);
	auto msg = std::make_shared<Proto_msg>(1, 6);
	serialize_obj(msg->body, info);
	broadcast_event_in_room(room_id, msg);
}

void System::broadcast_event_in_room(int room_id, std::shared_ptr<Proto_msg> msg)
{
	const auto& users = room[room_id]->get_Room_user();
	for (const auto& n : users)
		session[n]->push_event(msg);
}

void System::delete_room(int room_id)
{
	room[room_id].reset();
}

void System::route()
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
		//case 1:µÇÂ¼·þÎñ
	case 1:
		login(msg);
		break;
	case 2:
		show_room(msg);
		break;
	case 3:
		create_room(msg);
		break;
	case 4:
		exit_room(msg);
		break;
	case 5:
		join_room(msg);
		break;
	case 6:
		exit_room(msg);
		break;
	case 7:
		set_ready(msg);
		break;
	case 8:
		start_game(msg);
		break;
	case 502:
		receive_room_server_start_ack(msg);
		break;
	case 1000:
		regist_room_server(msg);
		break;
	case 50000:
		close(msg);
		break;
	default:
		std::cerr << "undefined service packet\n";
	}
	io.post(bind(&System::route, shared_from_this()));
}