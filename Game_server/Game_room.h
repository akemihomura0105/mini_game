#pragma once
#include "User.h"
#include <set>
class Game_room
{
public:
	int add_user();
	int remove_user();
	int game_start();
	
private:
	std::set<User>room;
};

