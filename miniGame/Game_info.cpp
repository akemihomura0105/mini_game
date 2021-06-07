#include "Game_info.h"

std::ostream& operator<<(std::ostream& os, const basic_game_info& info)
{
	os << "当前阶段:\t";
	if (info.stage == basic_game_info::STAGE::READY)
		os << "准备阶段\n";
	if (info.stage == basic_game_info::STAGE::DEPATURE0)
		os << "移动阶段0\n";
	if (info.stage == basic_game_info::STAGE::DEPATURE1)
		os << "移动阶段1\n";
	std::string character_name;
	if (info.character_id == 1)
		character_name = "探险家\n";
	if (info.character_id == 2)
		character_name = "恶灵\n";

	os << "当前时间:\t" << info.now_time - info.today_time << "\n";
	os << "身份:\t" << character_name << "\n";
	os << "当前生命:\t" << info.HP << "\n";
	os << "当前地点:\t" << info.location << "\n";

	Otp_table other_info(4);
	other_info.insert({ "id","昵称","地点","生命" });
	for (int i = 0; i < info.player.size(); i++)
		other_info.insert({ std::to_string(i),info.player[i].name,std::to_string(info.player[i].location),std::to_string(info.player[i].HP) });
	os << other_info << "\n";
	return os;
}

void basic_game_info::update()
{
	using namespace stage_time;
	auto tmp = get_current_stage();
	if (stage == tmp)
		return;
	stage = get_current_stage();
	if (stage == STAGE::DEPATURE0)
	{
		std::cout << "行动阶段0\n";
		action_point = true;
		if (character_id != 1)
			std::cout << "请进行移动\n";
	}
	if (stage == STAGE::DEPATURE1)
	{
		std::cout << "行动阶段1\n";
		stage = STAGE::DEPATURE1;
		if (character_id == 1)
			std::cout << "请进行移动\n";
	}
	if (stage == STAGE::DAYTIME)
	{
		action_point = true;
		std::cout << "白天\n";
	}
	if (now_time % 5 == 0)
		std::cout << *this;
}

basic_game_info::STAGE basic_game_info::get_current_stage()const
{
	using namespace stage_time;
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
	return STAGE::NIGHT;
}