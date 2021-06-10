#pragma once
#include"Otp_table.h"
#include"../general_class/game_const_value.h"
#include"../general_class/Resource.h"
#include<chrono>
#include<vector>
#include<string>
struct player_info
{
	int session_id;
	std::string name;
	int HP = CONSTV::initial_HP;
	int location = 0;
};
struct basic_game_info
{
	int now_time = 0;
	int today_time = 0;
	int character_id, session_id;
	Resource res;
	int HP = CONSTV::initial_HP;
	int day = 0;
	int turn = 0;
	int action_point;
	int location = 0;
	int game_id = -1;
	enum class STAGE { READY, DEPATURE0, DEPATURE1, DAYTIME, NIGHT };
	STAGE stage;
	std::vector<player_info>player;
	friend std::ostream& operator<<(std::ostream& os, const basic_game_info& info);
	void next_stage();
private:
	STAGE get_current_stage()const;

};

class Treasure_hunter_game_info :public basic_game_info
{

};

