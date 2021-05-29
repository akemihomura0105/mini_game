#pragma once
#include <list>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/list.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "User_info.h"

struct Room_prop
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& name;
		ar& id;
		ar& size;
		ar& capacity;
	}
	std::string name;
	size_t id;
	size_t size;
	size_t capacity;
};

struct Room_info
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& name;
		ar& room_id;
		ar& capacity;
		ar& user;
	}
	std::string name;
	size_t room_id;
	size_t capacity;
	std::list<User_info>user;
};