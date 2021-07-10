#pragma once
#include "Character_factory.h"
#include "../general_class/Game_proto.h"
#include "../general_class/game_const_value.h"
#include "../general_class/Auction_list.h"
#include "../general_class/Room_info.h"
#include "../general_class/Tcp_connection.h"
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
public:
	Game_room(io_context& io, const Room_info& info);

	//开始游戏后所需函数---------------------------------------------------


	//监听回合，若当前回合已经进入结算，则处理数据包。
	bool listen();
	//弹出消息队列中的消息
	std::pair<int, std::shared_ptr<Proto_msg>>msg_pop();
	//根据会话号获取玩家信息
	std::shared_ptr<Actionable_character> get_player(int session_id);
	void change_location(std::shared_ptr<Proto_msg>);
	void attack(std::shared_ptr<Proto_msg>);
	void heal(std::shared_ptr<Proto_msg>);
	void mine(std::shared_ptr<Proto_msg>);
	void explore(std::shared_ptr<Proto_msg>);
	void bid(std::shared_ptr<Proto_msg>);
private:
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	std::unordered_map<int, std::shared_ptr<Tcp_connection>>session;
	Room_info room_info;
	io_context& io;
	std::mt19937 mt;
	std::list<int>users;
	int rome_owner = 0;
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

	std::queue<int>explore_que;
	typedef std::pair<int, int>treasure_P;
	std::vector<treasure_P>treasure_vec;

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
	void night_stage0(bool exec);
	void night_stage1(int bid_stage, bool exec);
	void settlement_stage();
	void broadcast_time();
	void broadcast_switch_stage();
	void broadcast_game_info();
	void broadcast_location(int location);
	void broadcast_ghost_sight();
	void broadcast_hp(int location);
	void broadcast_res(int session_id = -1);
	void broadcast_treasure_info();
	void broadcast_auction_item();
	void broadcast_buyer(int buyer, int price);
	void broadcast_base_info();

	void push_state_code(int session_id, const state_code& sc);

	void switch_stage_calc();
	void next_day();

	void route();
};