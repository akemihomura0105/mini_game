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
	room_info.insert({ "房间名","房间号","房间人数" });
	for (const auto& info : info_vec)
		room_info.insert({ info.name,std::to_string(info.id),std::to_string(info.size) + "/" + std::to_string(info.capacity) });
	std::cout << room_info;
}

int System::join_room(int state)
{
	Proto_msg req_msg(1, 5);
	std::cout << "请输入加入房间的房间号：\n";
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
	std::cout << "成功加入房间\n";
	this->room_id = room_id;
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
			std::cout << "人数超限\n" << please_input_std_format;
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
	std::cout << "房间已建立";
	room_info.insert({ "房间名", "房间号", "房间人数" });
	room_info.insert({ name,std::to_string(room_id),"1/" + std::to_string(capacity) });
	std::cout << room_info;
}

void System::exit_room()
{
	auto req_msg = std::make_shared<Proto_msg>(1, 4);
	serialize_obj(req_msg->body, session_id, room_id);
	conn->push_event(req_msg);
}

void System::run()
{
	sock->connect(ep);
	auto msg = get_msg();
	deserialize_obj(msg->body, session_id);
	std::cout << "建立了会话：" << session_id << std::endl;
	conn = std::make_shared<Tcp_connection>(io, sock, msg_que, session_id);
	while (login());
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
	case 52:
	{
		receive_state_code_result(msg);
		break;
	}
	case 53:
	{
		receive_location_info(msg);
		break;
	}
	case 54:
	{
		receive_hp_info(msg);
		break;
	}
	case 55:
	{
		receive_res_info(msg);
		break;
	}
	case 56:
	{
		receive_bid_info(msg);
		break;
	}
	case 57:
	{
		receive_buyer_info(msg);
		break;
	}
	case 58:
	{
		receive_stage_change(msg);
		break;
	}
	case 59:
	{
		receive_treasure_result(msg);
		break;
	}
	case 60:
	{
		receive_treasure_info(msg);
		break;
	}
	case 61:
	{
		game_finish(msg);
		break;
	}
	case 62:
	{
		receive_ghost_sight(msg);
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
	auto input_ptr = input_que.try_pop();
	if (input_ptr == nullptr)
	{
		io.post(bind(&System::message_route, shared_from_this()));
		return;
	}
	std::string_view input = *input_ptr;
	if (state == STATE::ROOM)
	{
		if (input == "q")
		{
			exit_room();
			state = STATE::HALL;
		}
		if (input == "sr")
			set_ready();
		if (input == "sg")
			start_game();
	}

	if (state == STATE::GAME)
	{
		if (input.size() >= 3 && input.substr(0, 2) == "mv")
		{
			int val = input[2] - '0';
			change_location(val);
		}
		if (input.size() >= 4 && input.substr(0, 3) == "atk")
		{
			int game_id = input[3] - '0';
			attack(game_id);
		}
		if (input.size() >= 5 && input.substr(0, 4) == "heal")
		{
			int game_id = input[4] - '0';
			heal(game_id);
		}
		if (input.size() == 4 && input.substr(0, 4) == "mine")
		{
			mine();
		}
		if (input.size() >= 3 && input.size() <= 10 && input.substr(0, 3) == "bid")
		{
			auto num_str = input.substr(3, input.size() - 3);
			int val = std::stoi(num_str.data());
			bid(val);
		}
		if (input.size() == 3 && input.substr(0, 3) == "exp")
		{
			explore();
		}
	}
	if (input == "?")
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
	tbl.insert({ "退出房间","q" });
	tbl.insert({ "准备/取消准备","sr" });
	tbl.insert({ "开始游戏","sg" });
	std::cout << tbl;
}

void System::otp_game_operation()
{

}

void System::hall_system_run()
{
	state = STATE::HALL;
	Otp_table main_screen(2);
	main_screen.insert({ "显示所有房间" ,"ls" });
	main_screen.insert({ "创建某个房间" ,"mk" });
	main_screen.insert({ "进入某个房间" ,"cd" });
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
	io.run();
}

void System::sync_time(std::shared_ptr<Proto_msg>msg)
{
	deserialize_obj(msg->body, game_info->now_time);
	//std::cout << "当前时间：" << game_info->now_time << "\n";
}

void System::game_system_run()
{
	state = STATE::GAME;
}

void System::update_room_info(std::shared_ptr<Proto_msg>msg)
{
	deserialize_obj(msg->body, room_info);
	Otp_table prop_table(3);
	prop_table.insert({ "名称","房间号","容量" });
	prop_table.insert({ room_info.name,std::to_string(room_info.room_id),std::to_string(room_info.user.size()) + "/" + std::to_string(room_info.capacity) });
	room_info.user;
	Otp_table user_table(2);
	user_table.insert({ "用户名","准备状态" });
	for (const auto& n : room_info.user)
		user_table.insert({ n.name, (n.ready ? "已准备" : "未准备") });
	std::cout << prop_table << user_table;
}

void System::read_input()
{
	if (state == STATE::HALL)
		return;
	std::string msg;
	getline(std::cin, msg);
	std::cerr << "接收到字符串:" << msg << std::endl;
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
	int character_id, hp;
	Resource res;
	deserialize_obj(msg->body, character_id, hp, res);
	std::cerr << character_id << "---------\n";
	game_info = std::make_shared<basic_game_info>();
	game_info->player.resize(room_info.user.size());
	int p = 0;
	for (auto ite = room_info.user.begin(); ite != room_info.user.end(); ite++, p++)
	{
		game_info->player[p].name = ite->name;
		game_info->player[p].session_id = ite->session_id;
		if (ite->session_id == session_id)
			game_info->game_id = p;
	}
	game_info->character_id = character_id;
	game_info->HP = hp;
	game_info->res = res;
	std::cout << *game_info << "\n";
}

bool System::daytime_action_check()
{
	if (!game_info->action_point)
	{
		std::cout << "没有足够的体力\n";
		return true;
	}
	if (game_info->stage != STAGE::DAYTIME)
	{
		std::cout << "现在不是白天\n";
		return true;
	}
	return false;
}

void System::change_location(int location)
{
	if (!game_info->action_point)
	{
		std::cout << "没有足够的体力\n";
		return;
	}
	if (game_info->location == location)
	{
		std::cout << "尝试移动至相同的地点\n";
		return;
	}
	auto msg = std::make_shared<Proto_msg>(1, 52);
	serialize_obj(msg->body, session_id, location, room_id);
	conn->push_event(msg);
}

void System::attack(int game_id)
{
	if (daytime_action_check())
		return;
	if (game_info->res.armo == 0)
	{
		std::cout << "没有弹药\n";
		return;
	}
	int des = game_info->player[game_id].session_id;
	auto msg = std::make_shared<Proto_msg>(1, 53);
	serialize_obj(msg->body, session_id, des, room_id);
	conn->push_event(msg);
}

void System::heal(int game_id)
{
	if (daytime_action_check())
		return;
	if (game_info->res.bandage == 0)
	{
		std::cout << "没有绷带\n";
		return;
	}
	int des = game_info->player[game_id].session_id;
	auto msg = std::make_shared<Proto_msg>(1, 54);
	serialize_obj(msg->body, session_id, des, room_id);
	conn->push_event(msg);
}

void System::mine()
{
	if (daytime_action_check())
		return;
	auto msg = std::make_shared<Proto_msg>(1, 55);
	serialize_obj(msg->body, session_id, room_id);
	conn->push_event(msg);
}

void System::explore()
{
	if (daytime_action_check())
		return;
	if (game_info->character_id != 1)
		return;
	auto msg = std::make_shared<Proto_msg>(1, 57);
	serialize_obj(msg->body, session_id, room_id);
	conn->push_event(msg);
}

void System::bid(int price)
{
	if (game_info->res.coin < price)
		std::cout << "没有足够的金钱";
	auto msg = std::make_shared<Proto_msg>(1, 56);
	serialize_obj(msg->body, session_id, room_id, price);
	conn->push_event(msg);
}

void System::receive_state_code_result(std::shared_ptr<Proto_msg> msg)
{
	state_code sc;
	deserialize_obj(msg->body, sc);
	if (sc == CODE::MOVE_SUCCESS)
	{
		game_info->action_point = false;
	}
	if (sc == CODE::ATTACK_SUCCESS)
	{
		game_info->action_point = false;
		game_info->res.armo--;
	}
	if (sc == CODE::HEAL_SUCCESS)
	{
		game_info->action_point = false;
		game_info->res.bandage--;
	}
	if (sc == CODE::MINE_SUCCESS)
	{
		game_info->action_point = false;
		game_info->res.coin += CONSTV::MINE_COIN;
	}
	if (sc == CODE::EXPLORE_RECEIVE)
		game_info->action_point = false;
	std::cout << sc.message() << "\n";
}

void System::receive_hp_info(std::shared_ptr<Proto_msg> msg)
{
	typedef std::pair<int, int>life_info;
	std::vector<life_info>hp_set;
	deserialize_obj(msg->body, hp_set);
	for (const auto& p : hp_set)
	{
		if (game_info->game_id == p.first)
			game_info->HP = p.second;
		game_info->player[p.first].HP = p.second;
	}
}

void System::receive_location_info(std::shared_ptr<Proto_msg>msg)
{
	std::vector<int>location_set;
	deserialize_obj(msg->body, location_set);
	for (auto& n : game_info->player)
		n.location = -1;
	for (int i = 1; i < location_set.size(); i++)
	{
		if (location_set[i] == game_info->game_id)
			game_info->location = location_set[0];
		game_info->player[location_set[i]].location = location_set[0];
	}
}

void System::receive_ghost_sight(std::shared_ptr<Proto_msg> msg)
{
	std::vector<int>location_set;
	deserialize_obj(msg->body, location_set);
	for (int i = 0; i < game_info->player.size(); i++)
		game_info->player[i].location = location_set[i];
}

void System::receive_res_info(std::shared_ptr<Proto_msg>msg)
{
	deserialize_obj(msg->body, game_info->res);
}

void System::receive_treasure_result(std::shared_ptr<Proto_msg> msg)
{
	state_code sc;
	deserialize_obj(msg->body, sc);
	if (sc == CODE::EXPLORE_SUCCESS)
	{
		std::cout << "成功获取线索\n";
		game_info->res.hint++;
	}
	else
		std::cout << "未能获取线索\n";
}

void System::receive_treasure_info(std::shared_ptr<Proto_msg> msg)
{
	typedef std::pair<int, int>P;
	std::vector<P>treasure_vec;
	deserialize_obj(msg->body, treasure_vec);
	Otp_table table(2);
	table.insert({ "地点","宝藏编号" });
	for (const auto& p : treasure_vec)
		table.insert({ std::to_string(p.first),std::to_string(p.second) });
	std::cout << table;
}

void System::receive_bid_info(std::shared_ptr<Proto_msg> msg)
{
	Auction_item item;
	deserialize_obj(msg->body, item);
	Otp_table table(3);
	table.insert({ "商品名", "价格", "当前竞拍人" });
	if (item.bidder != -1)
		table.insert({ Auction_item::item_name[item.item_id],std::to_string(item.price),game_info->player[item.bidder].name });
	else
		table.insert({ Auction_item::item_name[item.item_id],std::to_string(item.price),"无人竞拍" });
	std::cout << table;
}

void System::receive_buyer_info(std::shared_ptr<Proto_msg>msg)
{
	int game_id, price;
	deserialize_obj(msg->body, game_id, price);
	if (game_id == game_info->game_id)
		game_info->res.coin -= price;
	std::cout << game_info->player[game_id].name << " 购买了本件商品\n";
}

void System::receive_stage_change(std::shared_ptr<Proto_msg>msg)
{
	game_info->next_stage();
}

void System::game_finish(std::shared_ptr<Proto_msg> msg)
{
	std::cout << "游戏结束\n";
}

System::System(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep)
{
	sock = std::make_shared<ip::tcp::socket>(io);
}