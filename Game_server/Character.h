#pragma once
#include "../general_class/state_code.h"
class Actionable_character
{
	static constexpr int MAX_HP = 10;
public:
	void get_damage(int n);
	virtual state_code attack(Actionable_character& character, bool try_flag = false);
	state_code treasure_hunt(bool try_flag = false);
	state_code move(int target_location, bool try_flag = false);
	virtual state_code heal(Actionable_character& character, bool try_flag = false);
	state_code try_buy();
	virtual void next_turn();
	virtual void next_day();
private:
	int HP;
	int armo;
	int bandage;
	int coin;
	int location;
	int session_id;
	bool action_flag;
	bool alive_flag;
};

class Treasure_hunter :public Actionable_character
{
private:
	int hint;
public:
	void explore();
};

class Evil_spirit :public Actionable_character
{
public:
	state_code attack(Actionable_character& character, bool try_flag = false);
	void next_turn();
private:
	static constexpr int MAX_charge_num = 2;
	static constexpr int COOLDOWN_TIME = 3;
	int skill_charge_num;
	int cooldown_cnt;
};