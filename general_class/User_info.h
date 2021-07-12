#pragma once
#include<string>

struct User_info
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& session_id;
		ar& name;
		ar& ready;
	}
	int session_id;
	std::string name;
	bool ready;
	User_info();
	User_info(int session_id, std::string _name, bool _ready);
};