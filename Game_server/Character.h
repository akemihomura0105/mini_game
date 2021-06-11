#pragma once
#include "../general_class/state_code.h"
#include "../general_class/Resource.h"
#include "../general_class/game_const_value.h"
#include "../general_class/Auction_list.h"
#include <memory>
#include <algorithm>
#include <numeric>
#include <random>
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
	int get_hint()const;
	const Resource& get_res()const;
	void set_res(Resource& res);
	void clear_res();
	bool isalive()const;
	bool has_action_point()const;
	void set_character_id(int n);
	void set_action_flag(bool flag);
	void get_damage(int n);
	void add_armo(int n = 1);
	void add_bandage(int n = 1);
	void add_hint(int n = 1);
	state_code action_check();
	virtual state_code attack(Actionable_character& character, bool try_flag = false);
	state_code treasure_hunt(bool try_flag = false);
	state_code move(int target_location, bool try_flag = false);
	void move_force(int target_location);
	virtual state_code heal(Actionable_character& character, bool try_flag = false);
	state_code mine(bool try_flag = false);
	state_code bid(Auction_item& item, int price);

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
public:
	state_code explore(bool try_flag = false);
	Treasure_hunter(int game_id, int session_id);
};

class Evil_spirit :public Actionable_character
{
public:
	state_code attack(Actionable_character& character, bool try_flag = false);
	void next_turn();
	Evil_spirit(int room_id, int session_id);
private:
	int skill_charge_num;
	int cooldown_cnt;
};