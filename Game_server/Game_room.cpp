#include "Game_room.h"
ID_generator<int> Game_room::room_gen(1, 1000);

Game_room::Room_property::Room_property(bool init)
{
	if (init)
		id = room_gen.generate();
}

/*
* return 0: add normally
* return 1: full room
*/
int Game_room::add_user(int session_id, std::shared_ptr <User> _user)
{
	if (users.size() == prop.capacity)
		return 1;
	users.push_back(session_id);
	_user->set_room_id(get_id());
	return 0;
}


// return 0: delete normally
// return 1: users not found;
// return 2: empty room, should be deleted by system.

int Game_room::remove_user(int session_id, std::shared_ptr<User> _user)
{
	auto ite = std::find(users.begin(), users.end(), session_id);
	if (ite == users.end())
		return 1;
	users.erase(ite);
	if (users.empty())
		return 2;
	_user->set_room_id(0);
	return 0;
}

void Game_room::start_game()
{
	using namespace std::chrono;
	load_player();
	last_time = start_time = steady_clock::now();
	last_broadcast_time = 0s;
	io->post(bind(&Game_room::ready_stage, shared_from_this(), true));
	io->post(bind(&Game_room::broadcast_time, shared_from_this()));
}

int Game_room::get_homeowner()
{
	return users.front();
}

const Game_room::Room_property& Game_room::get_Room_property()const
{
	return prop;
}

int Game_room::get_id() const
{
	if (prop.id == nullptr)
		return 0;
	return *prop.id;
}

const std::list<int>& Game_room::get_Room_user() const
{
	return users;
}

Game_room::Game_room(io_context* _io) :io(_io) {}
Game_room::Game_room(const Room_property&& _prop, io_context* _io) : prop(_prop), io(_io) {}

bool Game_room::listen()
{
	return !msg_que.empty();
}

std::pair<int, std::shared_ptr<Proto_msg>> Game_room::msg_pop()
{
	auto pair = std::move(msg_que.front());
	msg_que.pop();
	return pair;
}

std::shared_ptr<Actionable_character> Game_room::get_player(int session_id)
{
	return player[session_id];
}

void Game_room::load_player()
{
	std::vector<int>vec;
	switch (prop.capacity)
	{
	default:
		break;
	case 1:
		vec = { 1 };
		location.resize(3);
		break;
	case 2:
		vec = { 1,2 };
		location.resize(3);
		break;
	case 4:
		vec = { 1,1,1,2 };
		location.resize(3);
		break;
	}
	std::shuffle(vec.begin(), vec.end(), std::default_random_engine(time(0)));
	auto uite = users.begin();
	auto vite = vec.begin();
	int game_id = 0;
	for (; uite != users.end(); uite++)
	{
		player[*uite] = Character_factory::create(*vite, game_id++, *uite);
		location[0].insert(player[*uite]);
		vite++;
	}
}

std::chrono::seconds Game_room::get_duration_since_last_stage()
{
	using namespace std::chrono;
	return duration_cast<seconds>(steady_clock::now() - last_time);
}

bool Game_room::switch_stage(std::chrono::seconds sec)
{
	using namespace std::chrono;
	auto duration = get_duration_since_last_stage();
	if (duration >= sec)
	{
		last_time = steady_clock::now();
		return true;
	}
	return false;
}

void Game_room::ready_stage(bool exec)
{
	using namespace std::chrono;
	if (exec)
	{
		std::cerr << "enter the ready stage" << std::endl;
		broadcast_character();
	}
	auto duration = get_duration_since_last_stage();
	if (switch_stage(7s))
		io->post(bind(&Game_room::depature_stage0, shared_from_this(), true));
	else
		io->post(bind(&Game_room::ready_stage, shared_from_this(), false));
}

void Game_room::depature_stage0(bool exec)
{
	using namespace std::chrono;
	if (exec)
	{
		std::cerr << "enter the depature stage0" << std::endl;
	}
	if (switch_stage(5s))
		io->post(bind(&Game_room::depature_stage1, shared_from_this(), true));
	else
		io->post(bind(&Game_room::depature_stage0, shared_from_this(), false));
}

void Game_room::depature_stage1(bool exec)
{

}

std::vector<int> Game_room::get_session_set(int _location)
{
	std::vector<int>session_set;
	for (const auto& p : location[_location])
		session_set.push_back(p->get_session_id());
	return session_set;
}

void Game_room::broadcast_time()
{
	//static int cnt = 0;
	using namespace std::chrono;
	auto msg = std::make_shared<Proto_msg>(1, 49);
	seconds interval = 1s;
	//std::cerr << cnt++ << std::endl;
	if ((steady_clock::now() - start_time) > last_broadcast_time + interval)
	{
		last_broadcast_time += interval;
		auto duration = get_duration_since_last_stage();
		serialize_obj(msg->body, (int)duration.count());
		for (const auto& n : player)
			msg_que.emplace(n.first, msg);
	}
	io->post(bind(&Game_room::broadcast_time, shared_from_this()));
}

void Game_room::broadcast_character()
{
	auto msg = std::make_shared<Proto_msg>(1, 51);
	for (const auto& p : player)
	{
		serialize_obj(msg->body, p.second->get_character_id());
		msg_que.emplace(p.first, msg);
	}
}

void Game_room::broadcast_location(int _location)
{
	auto session_set = get_session_set(_location);
	auto msg = std::make_shared<Proto_msg>(1, 52);
	serialize_obj(msg->body, session_set);
	for (int i : session_set)
		msg_que.emplace(i, msg);
}

void Game_room::broadcast_hp(int _location)
{
	typedef std::pair<int, int>life_info;
	std::vector<life_info>life_set;
	for (const auto& n : location[_location])
		life_set.emplace_back(n->get_session_id(), n->get_hp_id());
	auto msg = std::make_shared<Proto_msg>(1, 53);
	serialize_obj(msg->body, life_set);
	for (const auto& n : location[_location])
		msg_que.emplace(n->get_session_id(), msg);
}

void Game_room::push_state_code(int session_id, const state_code& sc)
{
	auto msg = std::make_shared<Proto_msg>(1, 52);
	serialize_obj(msg->body, sc);
	msg_que.emplace(session_id, msg);
}

void Game_room::change_location(int session_id, int location)
{
	auto sc = player[session_id]->move(location, true);
	push_state_code(session_id, sc);
	if (sc == CODE::MOVE_SUCCESS)
		move_que.emplace(session_id, location);
}

void Game_room::attack(int src, int des)
{
	auto sc = player[src]->attack(*player[des], true);
	push_state_code(src, sc);
	if (sc == CODE::ATTACK_SUCCESS)
		atk_que.emplace(src, des);
}

void Game_room::heal(int src, int des)
{
	auto sc = player[src]->heal(*player[des], true);
	push_state_code(src, sc);
	if (sc == CODE::HEAL_SUCCESS)
		heal_que.emplace(src, des);
}

void Game_room::mine(int session_id)
{
	auto sc = player[session_id]->mine(true);
	push_state_code(session_id, sc);
	if (sc == CODE::MINE_SUCCESS)
		mine_que.emplace(session_id);

}