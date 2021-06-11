#include "Game_info.h"

std::ostream& operator<<(std::ostream& os, const basic_game_info& info)
{
	std::string character_name;
	if (info.character_id == 1)
		character_name = "探险家\n";
	if (info.character_id == 2)
		character_name = "恶灵\n";

	os << "当前时间:\t" << info.now_time - info.today_time << "\n";
	os << "身份:\t" << character_name << "\n";
	Otp_table table(6);
	table.insert({ "生命", "地点", "弹药", "金币", "绷带" ,"线索" });

	table.insert({ (info.HP <= 0 ? "死亡" : std::to_string(info.HP)) ,
		std::to_string(info.location),
		std::to_string(info.res.armo),std::to_string(info.res.coin) ,std::to_string(info.res.bandage),std::to_string(info.res.hint) });
	os << table;
	Otp_table other_info(4);
	other_info.insert({ "id","昵称","地点","生命" });
	for (int i = 0; i < info.player.size(); i++)
		other_info.insert({ std::to_string(i),info.player[i].name,
			(info.player[i].location == -1 ? "未知" : std::to_string(info.player[i].location)),
			std::to_string(info.player[i].HP) });
	os << other_info;
	return os;
}

void basic_game_info::next_stage()
{
	static int last_turn_time = 0;
	using namespace CONSTV;
	if (stage == STAGE::READY)
	{
		stage = STAGE::DEPATURE0;
		std::cout << "行动阶段0\n";
		action_point = true;
		if (character_id != 1)
			std::cout << "请进行移动\n";
	}
	else if (stage == STAGE::DEPATURE0)
	{
		stage = STAGE::DEPATURE1;
		std::cout << "行动阶段1\n";
		stage = STAGE::DEPATURE1;
		if (character_id == 1)
			std::cout << "请进行移动\n";
	}
	else if (stage == STAGE::DEPATURE1)
	{
		stage = STAGE::DAYTIME;
		turn = 0;
		action_point = true;
		std::cout << "白天, 回合: " << turn << "\n";
	}
	else if (stage == STAGE::DAYTIME)
	{
		if (turn++ < CONSTV::day_turn)
		{
			action_point = true;
			std::cout << "白天, 回合: " << turn << "\n";
		}
		else
		{
			stage = STAGE::NIGHT0;
			std::cout << "夜晚一阶段\n";
		}
	}
	else if (stage == STAGE::NIGHT0)
	{
		std::cout << "夜晚二阶段\n";
	}
	else if (stage == STAGE::NIGHT1)
	{
		day++;
		stage = STAGE::READY;
		std::cout << "准备阶段, 第" << day + 1 << "天\n";
	}
	std::cout << *this;
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}

STAGE basic_game_info::get_current_stage()const
{
	using namespace CONSTV;
	auto judge_stage = [&](const seconds& l, const seconds& r)
	{
		return l.count() <= now_time - today_time && now_time - today_time < r.count();
	};
	if (judge_stage(0s, ready))
		return STAGE::READY;
	if (judge_stage(ready, depature0))
		return STAGE::DEPATURE0;
	if (judge_stage(depature0, depature1))
		return STAGE::DEPATURE1;
	if (judge_stage(depature1, daytime))
		return STAGE::DAYTIME;
	if (judge_stage(daytime, night0))
		return STAGE::NIGHT0;
	return STAGE::NIGHT1;
}