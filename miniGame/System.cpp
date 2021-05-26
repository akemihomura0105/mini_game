#include "System.h"
#include "tools.h"
#include <array>

int System::login()
{
	OTP("请输入用户名");
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
			std::cout << "登录成功\n";
			return 0;
		}

		if (sc == CODE::LOGIN_REPEATED)
		{
			std::cout << "你已经在其他地点登录\n";
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

int System::make_room(int state = 0)
{
	const std::string please_input_std_format = "请按正确格式输入,如：\n my room name+6\n";
	if (!state)
		std::cout << "请输入房间名+人数上限，如：\nmy room name+6\n";
	else
		std::cout << please_input_std_format;
	std::string user_input;
	while (!isalpha(std::cin.peek()))
		std::cin.get();
	std::getline(std::cin, user_input);
	size_t offset = user_input.find('+', 0);
	if (offset == std::string::npos)
		return make_room(1);
	if (offset == user_input.size() - 1)
		return make_room(2);
	size_t capacity = 0;
	for (int i = offset + 1; i < user_input.size(); i++)
	{
		if (!isdigit(user_input[i]))
			return make_room(3);
		capacity = capacity * 10 + user_input[i] - '0';
		constexpr size_t MAX_CAPACITY = 20;
		if (capacity > MAX_CAPACITY)
		{
			std::cout << "人数超限\n" << please_input_std_format;
			return make_room(3);
		}
	}
	Proto_msg req_msg(1, 3);
	std::string name = user_input.substr(0, offset);
	serialize_obj(req_msg.body, session_id, name, capacity);
	send_msg(req_msg);

	auto res_msg = get_msg();
	size_t room_id;
	deserialize_obj(res_msg->body, room_id);
	if (room_id == 0)
		std::cout << "房间名已存在\n";
	Otp_table room_info(3);
	std::cout << "房间已建立";
	room_info.insert({ "房间名", "房间号", "房间人数" });
	room_info.insert({ name,std::to_string(room_id),"1/" + std::to_string(capacity) });
	std::cout << room_info;
	system("pause");
}

int System::quit_room(int state)
{

	return 0;
}

void System::run()
{
	sock->connect(ep);
	auto msg = get_msg();
	deserialize_obj(msg->body, session_id);
	std::cout << "建立了会话：" << session_id << std::endl;
	while (login())
	{
		//do something
		login();
	}
	std::cout << "login successfully" << std::endl;

	Otp_table main_screen(2);
	main_screen.insert({ "显示所有房间" ,"ls" });
	main_screen.insert({ "创建某个房间" ,"mk" });
	main_screen.insert({ "进入某个房间" ,"cd" });
	std::cout << main_screen;

	std::string str;
	std::cin >> str;
	if (str == "ls")
	{
		Proto_msg req_msg(1, 2);
		serialize_obj(req_msg.body, session_id);
		auto res_msg = get_msg();
		std::vector<Room_info>info_vec;
		deserialize_obj(res_msg->body, info_vec);
		Otp_table room_info(4);
		room_info.insert({ "房间名","房间号","房间人数" });
		for (const auto& info : info_vec)
			room_info.insert({ info.name,std::to_string(info.id),std::to_string(info.size) + "/" + std::to_string(info.capacity) });
		std::cout << room_info;
	}
	if (str == "mk")
	{
		make_room();
	}
	if (str == "cd")
	{
	}


	system("pause");

}

System::System(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep)
{
	sock = std::make_shared<ip::tcp::socket>(io);
	//sock->async_connect(ep, boost::bind(login));
}