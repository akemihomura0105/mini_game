#include "Game_info.h"

std::ostream& operator<<(std::ostream& os, const basic_game_info& info)
{
	std::cout << "当前阶段:\n";
	if (info.stage == basic_game_info::STAGE::READY)
		std::cout << "准备阶段\n";
	if (info.stage == basic_game_info::STAGE::DEPATURE0)
		std::cout << "移动阶段0\n";
	if (info.stage == basic_game_info::STAGE::DEPATURE1)
		std::cout << "移动阶段1\n";
	std::string character_name;
	if (info.character_id == 1)
		character_name = "探险家\n";
	if (info.character_id == 2)
		character_name = "恶灵\n";

	auto otp_str = [](const std::string& str, int len)
	{
		std::cout << str;
		std::cout << len;
	};

	constexpr int character_name_len = 10;

	info.otp_current_turn();
	std::cout << "身份: " << character_name << "\n";
	std::cout << "当前生命: " << info.HP << "\n";
	std::cout << "当前地点: " << info.location << "\n";
	return os;
}

void basic_game_info::update()
{
	using namespace stage_time;
	auto judge_stage = [&](const seconds& l, const seconds& r)
	{
		return l.count() <= now_time && now_time < r.count();
	};
	if (judge_stage(ready, depature0))
	{
		if (stage != STAGE::DEPATURE0)
		{
			std::cout << "行动阶段0\n";
			action_point = true;
		}
		stage = STAGE::DEPATURE0;
		if (character_id != 1)
			std::cout << "请进行移动\n";
	}
	if (judge_stage(depature0, depature1))
	{
		if (stage != STAGE::DEPATURE1)
		{
			std::cout << "行动阶段1\n";
		}
		stage = STAGE::DEPATURE1;
		if (character_id == 1)
			std::cout << "请进行移动\n";
	}
	if (now_time % 5 == 0)
		std::cout << *this;
}


void basic_game_info::otp_current_turn()const
{
	std::cout << "当前时间: " << now_time << std::endl;
}