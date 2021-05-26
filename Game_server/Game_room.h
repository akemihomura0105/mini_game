#pragma once
#include "User.h"
#include "ID_generator.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <set>
#include <map>
class Game_room
{
	friend class Room_info;
public:
	int add_user(size_t session_id);
	int remove_user(size_t session_id);
	int game_start();

	struct Room_info
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
		Room_info();
	};
	const Room_info& get_room_info()const;
	size_t get_id()const;

	bool operator<(const Game_room& room)const;
	Game_room();
	Game_room(const Room_info&& info);
	static ID_generator<size_t> room_gen;
private:
	Room_info info;
	std::set<size_t>users;
	size_t rome_owner;
};
