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

int Actionable_character::get_hp() const
{
	return HP;
}

int Actionable_character::get_armo()const
{
	return res.armo;
}

int Actionable_character::get_bandage()const
{
	return res.bandage;
}

int Actionable_character::get_location()const
{
	return location;
}

const Resource& Actionable_character::get_res()const
{
	return res;
}

void Actionable_character::set_res(Resource& _res)
{
	res = _res;
}

void Actionable_character::clear_res()
{
	res.clear();
}

bool Actionable_character::isalive() const
{
	return HP > 0;
}

bool Actionable_character::has_action_point() const
{
	return action_flag;
}

void Actionable_character::set_character_id(int n)
{
	character_id = n;
}

void Actionable_character::get_damage(int n)
{
	HP -= n;
}

void Actionable_character::add_armo(int n)
{
	res.armo += n;
}

void Actionable_character::add_bandage(int n)
{
	res.bandage += n;
}

state_code Actionable_character::action_check()
{
	state_code sc;
	if (!action_flag)
		sc.set(CODE::NO_ACTION);
	if (!isalive())
		sc.set(CODE::ALREADY_DEAD);
	return sc;
}

state_code Actionable_character::attack(Actionable_character& character, bool try_flag)
{
	state_code sc;
	sc = action_check();
	if (sc != CODE::NONE)
		return sc;
	if (res.armo == 0)
	{
		sc.set(CODE::NO_ARMO);
		return sc;
	}
	if (!character.isalive())
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
	sc = action_check();
	if (sc != CODE::NONE)
		return sc;
	if (!try_flag)
	{
		res.coin += bonus;
		action_flag = false;
	}
}

state_code Actionable_character::move(int target_location, bool try_flag)
{
	state_code sc;
	sc = action_check();
	if (sc != CODE::NONE)
		return sc;
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
	sc = action_check();
	if (sc != CODE::NONE)
		return sc;
	if (!res.bandage)
	{
		sc.set(CODE::NO_BANDAGE);
		return sc;
	}
	if (character.HP == CONSTV::MAX_HP)
	{
		sc.set(CODE::OBJECT_HAS_FULL_HP);
		return sc;
	}
	if (!character.isalive())
	{
		sc.set(CODE::OBJECT_HAS_DEAD);
		return sc;
	}
	sc.set(HEAL_SUCCESS);
	if (!try_flag)
	{
		res.bandage--;
		character.HP++;
	}
	return sc;
}

state_code Actionable_character::mine(bool try_flag)
{
	state_code sc;
	sc = action_check();
	if (sc != CODE::NONE)
		return sc;
	sc.set(CODE::MINE_SUCCESS);
	if (!try_flag)
		res.coin += 3;
	return sc;
}

state_code Actionable_character::bid(Auction_item& item, int price)
{
	state_code sc;
	if (price > get_res().coin)
	{
		sc.set(CODE::NO_ENOUGH_MONEY);
		return sc;
	}
	if (item.price >= price)
	{
		sc.set(CODE::NO_ENOUGH_MONEY);
		return sc;
	}
	item.price = price;
	item.bidder = game_id;
	sc.set(CODE::BID_SUCCESS);
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

Actionable_character::Actionable_character(int _game_id, int _session_id, int _HP, Resource&& _res) :
	game_id(_game_id), session_id(_session_id), HP(_HP), res(_res)
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
		int now_armo = get_res().armo;
		add_armo();
		auto sc = Actionable_character::attack(character, try_flag);
		if (sc != CODE::ATTACK_SUCCESS)
			add_armo(-1);
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

int Treasure_hunter::get_hint() const
{
	return hint;
}

state_code Treasure_hunter::explore(bool try_flag)
{
	state_code sc;
	sc = action_check();
	if (sc != CODE::NONE)
		return sc;
	sc.set(CODE::EXPLORE_RECEIVE);
	if (!try_flag)
	{
		static std::mt19937 mt;
		std::uniform_real_distribution dist(0.0, 1.0);
		if (dist(mt) < CONSTV::explore_probability)
		{
			sc.set(CODE::EXPLORE_SUCCESS);
			hint++;
		}
		else
		{
			sc.set(CODE::EXPLORE_FAILED);
		}
	}
	return sc;
}

Treasure_hunter::Treasure_hunter(int game_id, int session_id) :Actionable_character(
	game_id, session_id, CONSTV::initial_HP,
	Resource(CONSTV::TH_initial_coin, CONSTV::TH_initial_armo, CONSTV::TH_initial_bandage))
{
	set_character_id(1);

}

Evil_spirit::Evil_spirit(int game_id, int session_id) : Actionable_character(
	game_id, session_id, CONSTV::initial_HP,
	Resource(CONSTV::ES_initial_coin, CONSTV::ES_initial_armo, CONSTV::ES_initial_bandage))
{
	set_character_id(2);
}