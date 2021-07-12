#pragma once
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
struct Room_info
{
	friend class boost::serialization::access;
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