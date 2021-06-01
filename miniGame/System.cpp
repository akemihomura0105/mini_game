#include "System.h"
#include "tools.h"
#include <array>

int System::login()
{
	OTP("�������û���");
	std::string username;
	std::cin >> username;
	Proto_msg msg(1, 1);
	serialize_obj(msg.body, session_id, username);
	auto ec = send_msg(msg);
	if (ec)
		std::cout << ec.message();
	else
	{
		auto reply_msg_ptr = get_msg();
		state_code sc;
		deserialize_obj(reply_msg_ptr->body, sc);
		if (sc == CODE::LOGIN_SUCCESS)
		{
			std::cout << "��¼�ɹ�\n";
			return 0;
		}

		if (sc == CODE::LOGIN_REPEATED)
		{
			std::cout << "���Ѿ��������ص��¼\n";
			return 1;
		}
	}
}

std::shared_ptr<Proto_msg> System::get_msg()
{
	auto msg_ptr = std::make_shared<Proto_msg>(1);
	boost::system::error_code ec;
	read(*sock, buffer(&msg_ptr->head, sizeof(Proto_head)), ec);
	if (ec)
		std::cerr << ec.message() << std::endl;
	std::cerr << msg_ptr->head.service << "\n";
	std::cerr << msg_ptr->head.len << std::endl;
	msg_ptr->body.resize(msg_ptr->head.len);
	read(*sock, buffer(msg_ptr->body), ec);
	if (ec)
		std::cerr << ec.message() << std::endl;
	return msg_ptr;
}

boost::system::error_code System::send_msg(Proto_msg msg)
{
	boost::system::error_code ec;
	write(*sock, buffer(*msg.encode()), ec);
	return ec;
}

void System::show_room()
{
	Proto_msg req_msg(1, 2);
	serialize_obj(req_msg.body, session_id);
	send_msg(req_msg);
	auto res_msg = get_msg();
	std::vector<Room_prop>info_vec;
	state_code sc;
	std::cerr << res_msg->body << std::endl;
	deserialize_obj(res_msg->body, sc, info_vec);
	Otp_table room_info(3);
	room_info.insert({ "������","�����","��������" });
	for (const auto& info : info_vec)
		room_info.insert({ info.name,std::to_string(info.id),std::to_string(info.size) + "/" + std::to_string(info.capacity) });
	std::cout << room_info;
}

int System::join_room(int state)
{
	Proto_msg req_msg(1, 5);
	std::cout << "��������뷿��ķ���ţ�\n";
	int room_id;
	std::cin >> room_id;
	serialize_obj(req_msg.body, session_id, room_id);
	send_msg(req_msg);

	auto res_msg = get_msg();
	state_code sc;
	deserialize_obj(res_msg->body, sc);
	if (sc != CODE::NONE)
	{
		std::cout << sc.message();
		return join_room(0);
	}
	std::cout << "�ɹ����뷿��\n";
	this->room_id = room_id;
}

int System::make_room(int state = 0)
{
	const std::string please_input_std_format = "�밴��ȷ��ʽ����,�磺\n my room name+6\n";
	if (!state)
		std::cout << "�����뷿����+�������ޣ��磺\nmy room name+6\n";
	else
		std::cout << please_input_std_format;
	std::string user_input;
	while (!isalpha(std::cin.peek()))
		std::cin.get();
	std::getline(std::cin, user_input);
	int offset = user_input.find('+', 0);
	if (offset == std::string::npos)
		return make_room(1);
	if (offset == user_input.size() - 1)
		return make_room(2);
	int capacity = 0;
	for (int i = offset + 1; i < user_input.size(); i++)
	{
		if (!isdigit(user_input[i]))
			return make_room(3);
		capacity = capacity * 10 + user_input[i] - '0';
		constexpr int MAX_CAPACITY = 20;
		if (capacity > MAX_CAPACITY)
		{
			std::cout << "��������\n" << please_input_std_format;
			return make_room(3);
		}
	}
	Proto_msg req_msg(1, 3);
	std::string name = user_input.substr(0, offset);
	serialize_obj(req_msg.body, session_id, name, capacity);
	send_msg(req_msg);

	auto res_msg = get_msg();
	deserialize_obj(res_msg->body, room_id);
	Otp_table room_info(3);
	std::cout << "�����ѽ���";
	room_info.insert({ "������", "�����", "��������" });
	room_info.insert({ name,std::to_string(room_id),"1/" + std::to_string(capacity) });
	std::cout << room_info;
}

void System::exit_room()
{
	auto req_msg = std::make_shared<Proto_msg>(1, 4);
	std::cout << session_id << " " << room_id << std::endl;
	serialize_obj(req_msg->body, session_id, room_id);
	conn->push_event(req_msg);
}

void System::run()
{
	sock->connect(ep);
	auto msg = get_msg();
	deserialize_obj(msg->body, session_id);
	std::cout << "�����˻Ự��" << session_id << std::endl;
	conn = std::make_shared<Tcp_connection>(io, sock, msg_que, session_id);
	while (login())
	{
		//do something
		login();
	}
	std::cout << "login successfully" << std::endl;
	hall_system_run();
}

ASYNC_RET System::route()
{
	if (state != STATE::ROOM && state != STATE::GAME)
		return;
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
	case 6:
	{
		update_room_info(msg);
		break;
	}
	case 49:
	{
		sync_time(msg);
		break;
	}
	case 50:
	{
		game_system_run();
		break;
	}
	case 51:
	{
		create_game_info(msg);
		break;
	}
	default:
		std::cerr << "unknown package" << std::endl;
	}
	io.post(bind(&System::route, shared_from_this()));
}

ASYNC_RET System::message_route()
{
	if (state == STATE::HALL)
		return hall_system_run();
	auto input = input_que.try_pop();
	if (input == nullptr)
	{
		io.post(bind(&System::message_route, shared_from_this()));
		return;
	}
	if (state == STATE::ROOM)
	{
		if (*input == "q")
		{
			exit_room();
			state = STATE::HALL;
		}
		if (*input == "sr")
			set_ready();
		if (*input == "sg")
			start_game();
	}

	if (state == STATE::GAME)
	{
		
	}





	if (*input == "?")
	{
		if (state == STATE::ROOM)
			otp_room_operation();
		if (state == STATE::GAME)
			otp_game_operation();
	}
	io.post(bind(&System::message_route, shared_from_this()));
}

void System::otp_room_operation()
{
	Otp_table tbl(2);
	tbl.insert({ "�˳�����","q" });
	tbl.insert({ "׼��/ȡ��׼��","sr" });
	tbl.insert({ "��ʼ��Ϸ","sg" });
	std::cout << tbl;
}

void System::otp_game_operation()
{

}

void System::hall_system_run()
{
	state = STATE::HALL;
	Otp_table main_screen(2);
	main_screen.insert({ "��ʾ���з���" ,"ls" });
	main_screen.insert({ "����ĳ������" ,"mk" });
	main_screen.insert({ "����ĳ������" ,"cd" });
	std::cout << main_screen;

	std::string str;
	std::cin >> str;
	if (str == "ls")
	{
		show_room();
		system("pause");
		return hall_system_run();
	}
	if (str == "mk")
	{
		make_room();
		return room_system_run();
	}
	if (str == "cd")
	{
		join_room(0);
		return room_system_run();
	}
	hall_system_run();
}

void System::room_system_run()
{
	state = STATE::ROOM;
	conn->run();
	io.post(bind(&System::route, shared_from_this()));
	io.post(bind(&System::message_route, shared_from_this()));
	//io.post(bind(&System::read_input, shared_from_this()));
	std::thread input_t(bind(&System::read_input, shared_from_this()));
	input_t.detach();
}

void System::sync_time(std::shared_ptr<Proto_msg>msg)
{
	std::cout << msg->body;
	deserialize_obj(msg->body, game_info->now_time);
	game_info->update();
}

void System::game_system_run()
{
	state = STATE::GAME;
}

void System::update_room_info(std::shared_ptr<Proto_msg>msg)
{
	deserialize_obj(msg->body, room_info);
	Otp_table prop_table(3);
	prop_table.insert({ "����","�����","����" });
	prop_table.insert({ room_info.name,std::to_string(room_info.room_id),std::to_string(room_info.user.size()) + "/" + std::to_string(room_info.capacity) });
	room_info.user;
	Otp_table user_table(2);
	user_table.insert({ "�û���","׼��״̬" });
	for (const auto& n : room_info.user)
		user_table.insert({ n.name, (n.ready ? "��׼��" : "δ׼��") });
	std::cout << prop_table << user_table;
}

void System::read_input()
{
	if (state == STATE::HALL)
		return;
	std::string msg;
	getline(std::cin, msg);
	std::cerr << "���յ��ַ���:" << msg << std::endl;
	input_que.push(std::move(msg));
	//io.post(bind(&System::read_input, shared_from_this()));
	read_input();
}

void System::set_ready()
{
	auto req_msg = std::make_shared<Proto_msg>(1, 7);
	serialize_obj(req_msg->body, session_id, room_id);
	conn->push_event(req_msg);
}

void System::start_game()
{
	auto req_msg = std::make_shared<Proto_msg>(1, 8);
	serialize_obj(req_msg->body, session_id, room_id);
	conn->push_event(req_msg);
}

void System::create_game_info(std::shared_ptr<Proto_msg>msg)
{
	int character_id;
	deserialize_obj(msg->body, character_id);
	game_info = std::make_shared<basic_game_info>();
	game_info->player.resize(room_info.user.size());
	int p = 0;
	for (auto ite = room_info.user.begin(); ite != room_info.user.end(); ite++, p++)
	{
		game_info->player[p].name = ite->name;
		game_info->player[p].session_id = ite->session_id;
	}
	game_info->character_id = character_id;
	std::cout << *game_info << "\n";
}

System::System(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep)
{
	sock = std::make_shared<ip::tcp::socket>(io);
}