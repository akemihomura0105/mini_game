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
	prop.size++;
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
	prop.size--;
	if (users.empty())
		return 2;
	_user->set_room_id(0);
	return 0;
}

void Game_room::start_game()
{
	using namespace std::chrono;
	load_player();
	today_time = start_time = steady_clock::now();
	last_broadcast_time = 0s;
	atk_graph.resize(prop.capacity);
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

Game_room::Game_room(io_context* _io) :io(_io), resource_distributor(0) {}
Game_room::Game_room(const Room_property&& _prop, io_context* _io) : prop(_prop), io(_io), resource_distributor(_prop.capacity) {}

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
	order_player.reserve(prop.capacity);
	for (; uite != users.end(); uite++, game_id++)
	{
		player[*uite] = Character_factory::create(*vite, game_id, *uite);
		session_to_game[*uite] = game_id;
		order_player.push_back(player[*uite]);
		location[0].insert(player[*uite]);
		vite++;
	}
}

std::chrono::seconds Game_room::get_duration()
{
	using namespace std::chrono;
	return duration_cast<seconds>(steady_clock::now() - start_time);
}

void Game_room::ready_stage(bool exec)
{
	if (exec)
	{
		std::cerr << "enter the ready stage" << std::endl;
		broadcast_game_info();
	}
	if (get_current_stage() == STAGE::DEPATURE0)
	{
		switch_stage_calc();
		io->post(bind(&Game_room::depature_stage0, shared_from_this(), true));
	}
	else
		io->post(bind(&Game_room::ready_stage, shared_from_this(), false));
}

void Game_room::depature_stage0(bool exec)
{
	if (exec)
	{
		std::cerr << "enter the depature stage0" << std::endl;
	}
	if (get_current_stage() == STAGE::DEPATURE1)
	{
		switch_stage_calc();
		io->post(bind(&Game_room::depature_stage1, shared_from_this(), true));
	}
	else
		io->post(bind(&Game_room::depature_stage0, shared_from_this(), false));
}

void Game_room::depature_stage1(bool exec)
{
	if (exec)
	{
		std::cerr << "enter the depature stage1" << std::endl;
	}
	if (get_current_stage() == STAGE::DAYTIME)
	{
		switch_stage_calc();
		io->post(bind(&Game_room::daytime_stage, shared_from_this(), true));
	}
	else
		io->post(bind(&Game_room::depature_stage1, shared_from_this(), false));
}

void Game_room::daytime_stage(bool exec)
{
	if (exec)
	{
		std::cerr << "enter the daytime" << std::endl;
	}
	auto dura = get_duration();
	if (dura != last_turn_time && ((dura - CONSTV::depature1).count() % CONSTV::turn_duration == 0))
	{
		last_turn_time = dura;
		switch_stage_calc();
	}
	if (get_current_stage() == STAGE::NIGHT)
	{
		io->post(bind(&Game_room::night_stage, shared_from_this(), true));
	}
	else
		io->post(bind(&Game_room::daytime_stage, shared_from_this(), false));
}

void Game_room::night_stage(bool exec)
{

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
		serialize_obj(msg->body, (int)get_duration().count());
		for (const auto& n : player)
			msg_que.emplace(n.first, msg);
	}
	io->post(bind(&Game_room::broadcast_time, shared_from_this()));
}

void Game_room::broadcast_game_info()
{
	for (const auto& p : player)
	{
		auto msg = std::make_shared<Proto_msg>(1, 51);
		serialize_obj(msg->body, p.second->get_character_id(), p.second->get_hp(), p.second->get_res());
		std::cerr << p.first << "-----" << msg->body << "-----" << p.second->get_character_id() << std::endl;
		msg_que.push(std::make_pair(p.first, msg));
		msg_que.emplace(p.first, msg);
	}
}

void Game_room::broadcast_location(int _location)
{
	std::vector<int>location_set;
	location_set.push_back(_location);
	for (const auto& n : location[_location])
		location_set.push_back(n->get_game_id());
	auto msg = std::make_shared<Proto_msg>(1, 53);
	serialize_obj(msg->body, location_set);
	for (const auto& n : location[_location])
	{
		int session_id = n->get_session_id();
		msg_que.emplace(session_id, msg);
	}
}

void Game_room::broadcast_hp(int _location)
{
	typedef std::pair<int, int>life_info;
	std::vector<life_info>life_set;
	for (const auto& n : location[_location])
		life_set.emplace_back(n->get_game_id(), n->get_hp());
	auto msg = std::make_shared<Proto_msg>(1, 54);
	serialize_obj(msg->body, life_set);
	for (const auto& n : location[_location])
		msg_que.emplace(n->get_session_id(), msg);
}

void Game_room::broadcast_base_info()
{
	for (int i = 0; i < location.size(); i++)
	{
		broadcast_hp(i);
		broadcast_location(i);
	}
}

void Game_room::push_state_code(int session_id, const state_code& sc)
{
	auto msg = std::make_shared<Proto_msg>(1, 52);
	serialize_obj(msg->body, sc);
	msg_que.emplace(session_id, msg);
}

void Game_room::change_location(int session_id, int location)
{
	state_code sc;
	auto stage = get_current_stage();
	if ((stage == STAGE::DEPATURE0 && std::dynamic_pointer_cast<Evil_spirit>(player[session_id]) != nullptr)
		|| stage == STAGE::DEPATURE1 && std::dynamic_pointer_cast<Evil_spirit>(player[session_id]) == nullptr)
	{
		sc.set(NOT_YOUR_MOVE_TURN);
	}
	else
		sc = player[session_id]->move(location, true);
	push_state_code(session_id, sc);
	if (sc == CODE::MOVE_SUCCESS)
		move_que.emplace(session_id, location);
}

void Game_room::attack(int src, int des)
{
	auto stage = get_current_stage();
	if (stage != STAGE::DAYTIME)
		return;
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

Game_room::STAGE Game_room::get_current_stage()
{
	using namespace std::chrono;
	auto s = get_duration();
	if (0s <= s && s < CONSTV::ready)
		return STAGE::READY;
	if (CONSTV::ready <= s && s < CONSTV::depature0)
		return STAGE::DEPATURE0;
	if (CONSTV::depature0 <= s && s < CONSTV::depature1)
		return STAGE::DEPATURE1;
	if (CONSTV::depature1 <= s && s < CONSTV::daytime)
		return STAGE::DAYTIME;
	return STAGE::NIGHT;
}

void Game_room::switch_stage_calc()
{
	if (get_current_stage() == STAGE::DAYTIME)
	{
		for (auto& p : player)
			p.second->next_turn();
	}
	while (!heal_que.empty())
	{
		auto& [src, des] = heal_que.front();
		player[src]->heal(*player[des]);
		heal_que.pop();
	}
	while (!atk_que.empty())
	{
		for (int i = 0; i < player.size(); i++)
			atk_graph[i].clear();
		auto& [src, des] = atk_que.front();
		atk_graph[session_to_game[src]].push_back(session_to_game[des]);
		player[src]->attack(*player[des]);
		resource_distributor.solve(&atk_graph, &order_player);
		atk_que.pop();
	}
	while (!mine_que.empty())
	{
		auto& n = mine_que.front();
		player[n]->mine();
		mine_que.pop();
	}
	while (!move_que.empty())
	{
		auto& [src, des] = move_que.front();
		location[player[src]->get_location()].erase(player[src]);
		location[des].insert(player[src]);
		player[src]->move(des);
		move_que.pop();
	}
	broadcast_base_info();
}