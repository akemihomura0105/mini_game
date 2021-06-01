#include "Game_info.h"

std::ostream& operator<<(std::ostream& os, const basic_game_info& info)
{
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
	std::cout << *this;
}

void basic_game_info::otp_current_turn()const
{
	std::cout << "��ǰʱ��: " << now_time << std::endl;
}