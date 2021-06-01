#pragma once
#include"Otp_table.h"
#include<chrono>
#include<vector>
#include<string>
struct player_info
{
	int session_id;
	std::string name;
	int HP = 10;
	int location = 0;
};
struct basic_game_info
{
	int now_time = 0;
	int character_id, session_id;
	int coin;
	int armo;
	int HP = 10;
	int day = 0;
	int turn = 0;
	int action_point;
	int location = 0;
	enum class STAGE { READY, DAYTIME, NIGHT };
	STAGE stage;
	std::vector<player_info>player;
	friend std::ostream& operator<<(std::ostream& os, const basic_game_info& info);
	void update();
private:
	void otp_current_turn()const;

};

class Treasure_hunter_game_info :public basic_game_info
{

};

