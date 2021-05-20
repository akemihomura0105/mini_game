#pragma once
class Actionable_character
{
	int HP;
	int armo;
	int bandage;
	int coin;
	int id;

	

public:
	void get_damage();
	void attack();
	void treasure_hunt();
	void move();
	void heal();
	void try_buy();
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
private:
public:
};

