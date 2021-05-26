#include "Game_room.h"

ID_generator<size_t> Game_room::room_gen(1, 1000);

Game_room::Room_info::Room_info() { id = room_gen.generate(); }

int Game_room::add_user(size_t session_id)
{
	if (users.empty())
		rome_owner = session_id;
	users.insert(session_id);
	return 1;
}

int Game_room::remove_user(size_t session_id)
{
	users.erase(session_id);
	if (users.empty())
		return 1;
	return 0;
}

const Game_room::Room_info& Game_room::get_room_info()const
{
	return info;
}

size_t Game_room::get_id() const
{
	return *info.id;
}

bool Game_room::operator<(const Game_room& room) const
{
	return get_id() < room.get_id();
}

Game_room::Game_room(const Room_info&& _info) :info(_info) {}
