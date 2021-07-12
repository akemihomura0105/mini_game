#pragma once
#include "User.h"
#include "ID_generator.h"
#include "Character_factory.h"
#include "../general_class/Game_proto.h"
#include "../general_class/game_const_value.h"
#include "../general_class/Auction_list.h"
#include "Resource_distributor.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
#include <set>
#include <map>
#include <list>
#include <unordered_set>
#include <tuple>
#include <random>
#include <iostream>

class Game_room :public std::enable_shared_from_this<Game_room>
{
	friend class Room_property;

public:
	struct Room_property
	{
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& name;
			ar& int(*id);
			ar& size;
			ar& capacity;
		}
		//房间名
		std::string name;
		//房间id
		std::shared_ptr<ID<int>> id = nullptr;
		//房间当前用户数
		int size = 0;
		//房间容量
		int capacity;
		Room_property(bool init = false);
	};

	//传入session_id，和User指针，将用户添加至房间内，并修改用户的当前状态。
	int add_user(int session_id, std::shared_ptr<User> user);
	int remove_user(int session_id, std::shared_ptr<User> user);
	void start_game();
	int get_homeowner();

	//获取房间属性
	const Room_property& get_Room_property()const;
	//以链表形式获取用户列表
	const std::list<int>& get_Room_user()const;
	//获取房间id
	int get_id()const;
	Game_room(io_context* io);
	Game_room(const Room_property&& prop, io_context* io);
	static ID_generator<int> room_gen;

private:

	io_context* io;
	std::mt19937 mt;
	Room_property prop;
	std::list<int>users;
	int rome_owner = 0;
};