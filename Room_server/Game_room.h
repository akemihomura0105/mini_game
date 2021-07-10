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

	//��ʼ��Ϸ�����躯��---------------------------------------------------


	//�����غϣ�����ǰ�غ��Ѿ�������㣬�������ݰ���
	bool listen();
	//������Ϣ�����е���Ϣ
	std::pair<int, std::shared_ptr<Proto_msg>>msg_pop();
	//���ݻỰ�Ż�ȡ�����Ϣ
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

	//��Ϸ�׶Σ�
	// 	   1. ��ʼ�׶Σ���ʼ����ɫ��Ϣ��
	// 	   2. ��ʼ�ƶ�1�����������ɫѡ����Ҫ�ƶ��ĵص㡣
	// 	   3. ��ʼ�ƶ�2������ѡ����Ҫ�ƶ��ĵص㡣
	// 	   4. �ж����Խ׶Σ�ִ���ж�������
	// 	   5. �ж�����׶Σ���������->�˺�->�ڿ�->�ƶ���˳����㱾�غϴ�����ж���
	// 	   6. �ظ����ɴ�4,5�׶Ρ�
	// 	   7. �����׶Σ���Ʒ���������ת����2�׶Ρ�
	// 
	// 
	//��ʼ��Ϸ������Ҫ�����뺯��--------------------------------------------

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