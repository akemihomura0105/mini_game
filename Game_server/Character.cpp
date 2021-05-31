#include "Character.h"

void Actionable_character::get_damage(int n)
{
	HP -= n;
}

state_code Actionable_character::attack(Actionable_character& character, bool try_flag)
{
	state_code sc;
	if (!action_flag)
	{
		sc.set(CODE::NO_ACTION);
		return sc;
	}
	if (armo == 0)
	{
		sc.set(CODE::NO_ARMO);
		return sc;
	}
	if (!character.alive_flag)
	{
		sc.set(CODE::OBJECT_HAS_DEAD);
		return sc;
	}
	action_flag = false;
	character.get_damage(1);
	return sc;
}

state_code Actionable_character::treasure_hunt(bool try_flag)
{
	constexpr int bonus = 3;
	state_code sc;
	if (!action_flag)
	{
		sc.set(CODE::NO_ACTION);
		return sc;
	}
	coin += bonus;
	action_flag = false;
}

state_code Actionable_character::move(int target_location, bool try_flag)
{
	state_code sc;
	if (!action_flag)
	{
		sc.set(CODE::NO_ACTION);
		return sc;
	}
	if (location != target_location)
	{
		sc.set(CODE::MOVE_TO_SAME_LOCATION);
		return sc;
	}
	location = target_location;
	action_flag = false;
	return sc;
}

state_code Actionable_character::heal(Actionable_character& character, bool try_flag)
{
	state_code sc;
	if (!action_flag)
	{
		sc.set(CODE::NO_ACTION);
		return sc;
	}
	if (!bandage)
	{
		sc.set(CODE::NO_BANDAGE);
		return sc;
	}
	bandage--;
	character.HP++;
	return sc;
}

void Actionable_character::next_turn()
{
	action_flag = true;
}

state_code Evil_spirit::attack(Actionable_character& character, bool try_flag)
{
	state_code sc;
	if (!skill_charge_num)
	{
		sc.set(CODE::SKILL_STILL_IN_COOLDOWN);
		return sc;
	}
	skill_charge_num--;
	character.get_damage(1);
	return sc;
}

void Evil_spirit::next_turn()
{
	Actionable_character::next_turn();
	if (skill_charge_num == MAX_charge_num)
		return;
	if (--cooldown_cnt == 0)
	{
		skill_charge_num++;
		cooldown_cnt = COOLDOWN_TIME;
	}
}

void Treasure_hunter::explore()
{

}
