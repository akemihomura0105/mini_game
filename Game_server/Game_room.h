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

	//开始游戏后所需函数---------------------------------------------------


	//监听回合，若当前回合已经进入结算，则处理数据包。
	bool listen();
	//弹出消息队列中的消息
	std::pair<int, std::shared_ptr<Proto_msg>>msg_pop();
	//根据会话号获取玩家信息
	std::shared_ptr<Actionable_character> get_player(int session_id);
	void change_location(int session_id, int location);
	void attack(int src, int des);
	void heal(int src, int des);
	void mine(int session_id);
	void bid(int session_id, int price);
private:

	io_context* io;
	std::mt19937 mt;
	Room_property prop;
	std::list<int>users;
	int rome_owner = 0;
	enum class STAGE { READY, DEPATURE0, DEPATURE1, DAYTIME, NIGHT };
	STAGE stage;
	STAGE get_current_stage();
	int turn_num;

	//游戏阶段：
	// 	   1. 开始阶段：初始化角色信息。
	// 	   2. 初始移动1：恶灵以外角色选择所要移动的地点。
	// 	   3. 初始移动2：恶灵选择所要移动的地点。
	// 	   4. 行动宣言阶段：执行行动操作。
	// 	   5. 行动结算阶段：按照治疗->伤害->挖矿->移动的顺序结算本回合打出的行动。
	// 	   6. 重复若干次4,5阶段。
	// 	   7. 休整阶段：物品拍卖，随后转至第2阶段。
	// 
	// 
	//开始游戏后所需要数据与函数--------------------------------------------

	//Mapping from the session_id to the character.
	std::unordered_map<int, std::shared_ptr<Actionable_character>>player;

	std::vector<std::shared_ptr<Actionable_character>>order_player;
	std::unordered_map<int, int>session_to_game;
	//event que, the system will read from this queue to send message to client.
	std::queue<std::pair<int, std::shared_ptr<Proto_msg>>>msg_que;

	//heal tuple, define the heal action.@1 means src's session_id, @2means des's session_id.
	typedef std::tuple<int, int>heal_tuple;
	std::queue<heal_tuple>heal_que;
	//attack tuple, define the attack action.@1 means src's session_id, @2means des's session_id.
	typedef std::tuple<int, int>atk_tuple;
	std::queue<atk_tuple>atk_que;
	Resource_distributor resource_distributor;
	std::vector<std::list<int>>atk_graph;
	std::queue<int>mine_que;
	//move tuple, define the move action. @1 means src's session_id, @2means des's location id.
	typedef std::tuple<int, int>move_tuple;
	std::queue<move_tuple>move_que;

	Auction_item auction_item;

	//Mapping from the location to the character.
	std::vector<std::unordered_set<std::shared_ptr<Actionable_character>>>location;
	//Game start time.
	std::chrono::time_point<std::chrono::steady_clock>start_time;
	//End time of the last turn.
	std::chrono::seconds last_turn_time;
	std::chrono::seconds last_bid_time;
	std::chrono::time_point<std::chrono::steady_clock>today_time;

	std::chrono::milliseconds last_broadcast_time;

	//convert the list form to the unordered_map form.
	void load_player();
	std::chrono::seconds get_duration();
	void ready_stage(bool exec);
	void depature_stage0(bool exec);
	void depature_stage1(bool exec);
	void daytime_stage(bool exec);
	void night_stage(int bid_stage, bool exec);
	std::vector<int>get_session_set(int location);
	void broadcast_time();
	void broadcast_switch_stage();
	void broadcast_game_info();
	void broadcast_location(int location);
	void ghost_sight();
	void broadcast_hp(int location);
	void broadcast_res(int session_id = -1);
	void broadcast_auction_item();
	void broadcast_buyer(int buyer);
	void broadcast_base_info();

	void push_state_code(int session_id, const state_code& sc);

	void switch_stage_calc();
	void next_day();
};