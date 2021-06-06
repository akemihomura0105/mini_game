#include "Character.h"

int Actionable_character::get_session_id()const
{
	return session_id;
}

int Actionable_character::get_character_id()const
{
	return character_id;
}

int Actionable_character::get_game_id()const
{
	return game_id;
}

int Actionable_character::get_hp_id() const
{
	return HP;
}

int Actionable_character::get_location()const
{
	return location;
}

void Actionable_character::set_character_id(int n)
{
	character_id = n;
}

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
	sc.set(CODE::ATTACK_SUCCESS);
	if (!try_flag)
	{
		action_flag = false;
		character.get_damage(1);
	}
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
	if (!try_flag)
	{
		coin += bonus;
		action_flag = false;
	}
}

state_code Actionable_character::move(int target_location, bool try_flag)
{
	state_code sc;
	if (!action_flag)
	{
		sc.set(CODE::NO_ACTION);
		return sc;
	}
	if (location == target_location)
	{
		sc.set(CODE::MOVE_TO_SAME_LOCATION);
		return sc;
	}
	sc.set(CODE::MOVE_SUCCESS);
	if (!try_flag)
	{
		location = target_location;
		action_flag = false;
	}
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
	if (character.HP == character.MAX_HP)
	{
		sc.set(CODE::OBJECT_HAS_FULL_HP);
		return sc;
	}
	if (!character.alive_flag)
	{
		sc.set(CODE::OBJECT_HAS_DEAD);
		return sc;
	}
	if (!try_flag)
	{
		bandage--;
		character.HP++;
	}
	return sc;
}

state_code Actionable_character::mine(bool try_flag)
{
	state_code sc;
	if (!action_flag)
	{
		sc.set(CODE::NO_ACTION);
		return sc;
	}
	sc.set(CODE::MINE_SUCCESS);
	if (!try_flag)
		coin += 3;
	return sc;
}

void Actionable_character::next_turn()
{
	action_flag = true;
}

bool Actionable_character::operator==(const Actionable_character& character) const
{
	return session_id == character.session_id;
}

Actionable_character::Actionable_character(int _game_id, int _session_id) :game_id(_game_id), session_id(_session_id)
{
}

state_code Evil_spirit::attack(Actionable_character& character, bool try_flag)
{
	state_code sc;
	if (!skill_charge_num)
	{
		sc.set(CODE::SKILL_STILL_IN_COOLDOWN);
		return sc;
	}
	if (!try_flag)
	{
		skill_charge_num--;
		character.get_damage(1);
	}
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

Treasure_hunter::Treasure_hunter(int game_id, int session_id) :Actionable_character(game_id, session_id)
{
	set_character_id(1);
}

Evil_spirit::Evil_spirit(int game_id, int session_id) : Actionable_character(game_id, session_id)
{
	set_character_id(2);
}