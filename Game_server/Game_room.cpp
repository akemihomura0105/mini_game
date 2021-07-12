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
	_user->set_state(User::STATE::inroom);
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
	_user->set_state(User::STATE::online);
	return 0;
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