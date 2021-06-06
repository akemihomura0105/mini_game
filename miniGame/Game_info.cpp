#include "Game_info.h"

std::ostream& operator<<(std::ostream& os, const basic_game_info& info)
{
	std::cout << "��ǰ�׶�:\n";
	if (info.stage == basic_game_info::STAGE::READY)
		std::cout << "׼���׶�\n";
	if (info.stage == basic_game_info::STAGE::DEPATURE0)
		std::cout << "�ƶ��׶�0\n";
	if (info.stage == basic_game_info::STAGE::DEPATURE1)
		std::cout << "�ƶ��׶�1\n";
	std::string character_name;
	if (info.character_id == 1)
		character_name = "̽�ռ�\n";
	if (info.character_id == 2)
		character_name = "����\n";

	auto otp_str = [](const std::string& str, int len)
	{
		std::cout << str;
		std::cout << len;
	};

	constexpr int character_name_len = 10;

	info.otp_current_turn();
	std::cout << "���: " << character_name << "\n";
	std::cout << "��ǰ����: " << info.HP << "\n";
	std::cout << "��ǰ�ص�: " << info.location << "\n";
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
			std::cout << "�ж��׶�0\n";
			action_point = true;
		}
		stage = STAGE::DEPATURE0;
		if (character_id != 1)
			std::cout << "������ƶ�\n";
	}
	if (judge_stage(depature0, depature1))
	{
		if (stage != STAGE::DEPATURE1)
		{
			std::cout << "�ж��׶�1\n";
		}
		stage = STAGE::DEPATURE1;
		if (character_id == 1)
			std::cout << "������ƶ�\n";
	}
	if (now_time % 5 == 0)
		std::cout << *this;
}


void basic_game_info::otp_current_turn()const
{
	std::cout << "��ǰʱ��: " << now_time << std::endl;
}