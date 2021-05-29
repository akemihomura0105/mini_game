#pragma once
#include<string>

struct User_info
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& id;
		ar& name;
		ar& ready;
	}
	std::size_t id;
	std::string name;
	bool ready;
	User_info();
	User_info(size_t _id, std::string _name, bool _ready);
};