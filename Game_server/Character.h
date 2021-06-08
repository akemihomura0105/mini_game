#pragma once
#include "../general_class/state_code.h"
#include "../general_class/Resource.h"
#include "../general_class/game_const_value.h"
#include <memory>
class Actionable_character
{
public:
	int get_session_id()const;
	int get_character_id()const;
	int get_game_id()const;
	int get_hp()const;
	int get_armo()const;
	int get_bandage()const;
	int get_location()const;
	const Resource& get_res()const;
	void set_res(Resource& res);
	void clear_res();
	bool isalive()const;
	void set_character_id(int n);
	void get_damage(int n);
	virtual state_code attack(Actionable_character& character, bool try_flag = false);
	state_code treasure_hunt(bool try_flag = false);
	state_code move(int target_location, bool try_flag = false);
	virtual state_code heal(Actionable_character& character, bool try_flag = false);
	state_code mine(bool try_flag = false);
	//state_code try_buy();
	virtual void next_turn();
	//virtual void next_day();
	bool operator==(const Actionable_character& character)const;
	Actionable_character(int game_id, int session_id,
		int HP, Resource&& res);
private:
	int character_id;
	int game_id;
	int HP = CONSTV::initial_HP;
	Resource res;
	int location = 0;
	int session_id;
	bool action_flag = true;
};

class Treasure_hunter :public Actionable_character
{
private:
	int hint;
public:
	void explore();
	Treasure_hunter(int game_id, int session_id);
};

class Evil_spirit :public Actionable_character
{
public:
	state_code attack(Actionable_character& character, bool try_flag = false);
	void next_turn();
	Evil_spirit(int room_id, int session_id);
private:
	static constexpr int MAX_charge_num = 2;
	static constexpr int COOLDOWN_TIME = 3;
	int skill_charge_num;
	int cooldown_cnt;
};