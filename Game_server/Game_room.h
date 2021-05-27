#pragma once
#include "User.h"
#include "ID_generator.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/list.hpp>
#include <set>
#include <map>
#include <list>
class Game_room
{
	friend class Room_property;
public:
	int add_user(size_t session_id);
	int remove_user(size_t session_id);
	int game_start();

	struct Room_property
	{
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& name;
			ar& size_t(*id);
			ar& size;
			ar& capacity;
		}
		std::string name;
		std::shared_ptr<ID<size_t>> id;
		size_t size;
		size_t capacity;
		Room_property();
	};
	const Room_property& get_Room_property()const;
	std::list<size_t> get_Room_user()const;
	size_t get_id()const;

	bool operator==(const Game_room& room)const;
	Game_room();
	Game_room(const Room_property&& prop);
	static ID_generator<size_t> room_gen;
private:
	Room_property prop;
	std::list<size_t>users;
	size_t rome_owner = 0;
};

namespace std
{
	template<>
	struct hash<Game_room>
	{
		std::size_t operator()(const Game_room& key)const
		{
			return key.get_id();
		}
	};
}