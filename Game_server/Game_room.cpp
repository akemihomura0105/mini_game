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

/*
* return 0: delete normally
* return 1: users not found;
* return 2: empty room, should be deleted by system.
*/
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
	load_player();
	last_time = start_time = chrono::steady_clock::now();
	io.post(bind(&Game_room::ready_stage, shared_from_this()));
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

Game_room::Game_room(io_context& _io) :io(_io) {}
Game_room::Game_room(const Room_property&& _prop, io_context& _io) : prop(_prop), io(_io) {}

bool Game_room::listen()
{
	return listen_flag;
}

std::pair<int, std::shared_ptr<Proto_msg>> Game_room::msg_pop()
{
	if (msg_que.front().first == -1)
		listen_flag = false;
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
	case 4:
		vec = { 1,1,1,2 };
		location.resize(3);
		break;
	}
	std::shuffle(vec.begin(), vec.end(), std::default_random_engine(time(0)));
	auto uite = users.begin();
	auto vite = users.begin();
	for (; uite != users.end(); uite++)
	{
		player[*uite] = Character_factory::create(*vite);
		location[0].insert(*uite);
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
		listen_flag = true;
		msg_que.emplace(-1, nullptr);
		return true;
	}
	return false;
}

void Game_room::ready_stage(bool exec)
{
	using namespace std::chrono;
	if (exec)
	{
		broadcast_character();
		broadcast_location(true);
		broadcast_hp(true);
	}
	auto duration = get_duration_since_last_stage();
	if (switch_stage(5s))
		io.post(bind(&Game_room::depature_stage0, shared_from_this()));
	else
		io.post(bind(&Game_room::ready_stage, shared_from_this(), false));
}

void Game_room::depature_stage0(bool exec)
{
	using namespace std::chrono;
	if (exec)
	{

	}
	if (switch_stage(5s))
		io.post(bind(&Game_room::depature_stage1, shared_from_this()));
	else
		io.post(bind(&Game_room::depature_stage0, shared_from_this()));
}
