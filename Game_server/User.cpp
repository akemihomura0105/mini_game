#include "User.h"

int User::load_user(std::string username, std::string password)
{
	//Unrealized database operations. After these operations, the user info should be loaded into the object.
	//The following code will be used for bypass testing and will bypass all database related operations.
	//Begin
	//End
	return 0;

}

void User::set_id(int n)
{
	id = n;
}

bool User::operator<(const User& user) const
{
	return id < user.id;
}