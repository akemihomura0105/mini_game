#include "Game_room.h"

ID_generator<size_t> Game_room::room_gen(1, 1000);

Game_room::Room_property::Room_property() { id = room_gen.generate(); }

/*
* return 0: add normally
* return 1: full room
*/
int Game_room::add_user(size_t session_id)
{
	if (users.size() == prop.capacity)
		return 1;
	users.push_back(session_id);
	return 0;
}

/*
* return 0: delete normally
* return 1: users not found;
* return 2: empty room, should be deleted by system.
*/
int Game_room::remove_user(size_t session_id)
{
	auto ite = std::find(users.begin(), users.end(), session_id);
	if (ite == users.end())
		return 1;
	users.erase(ite);
	if (users.empty())
		return 2;
	return 0;
}

const Game_room::Room_property& Game_room::get_Room_property()const
{
	return prop;
}

size_t Game_room::get_id() const
{
	return *prop.id;
}

std::list<size_t> Game_room::get_Room_user() const
{
	return users;
}

bool Game_room::operator==(const Game_room& room) const
{
	return get_id() == room.get_id();
}

Game_room::Game_room() {}
Game_room::Game_room(const Room_property&& _prop) :prop(_prop) {}
