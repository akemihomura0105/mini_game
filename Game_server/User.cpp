#include "User.h"

int User::load_user(const std::string& _username, const std::string& _password)
{
	username = _username;
	state = STATE::online;
	//Unrealized database operations. After these operations, the user info should be loaded into the object.
	//The following code will be used for bypass testing and will bypass all database related operations.
	//Begin
	//End
	return 0;

}

const std::string& User::get_username() const
{
	return username;
}

void User::set_id(int n)
{
	id = n;
}

void User::set_room_id(int _room_id)
{
	room_id = _room_id;
}

int User::get_room_id()
{
	return room_id;
}

bool User::operator<(const User& user) const
{
	return id < user.id;
}

bool User::operator==(const User& user) const
{
	return get_username() == user.get_username();
}

bool User::is_ready()const
{
	return ready;
}

void User::set_ready()
{
	ready = !ready;
}

void User::set_state(STATE st)
{
	state = st;
}
