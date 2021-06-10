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
		//������
		std::string name;
		//����id
		std::shared_ptr<ID<int>> id = nullptr;
		//���䵱ǰ�û���
		int size = 0;
		//��������
		int capacity;
		Room_property(bool init = false);
	};

	//����session_id����Userָ�룬���û�����������ڣ����޸��û��ĵ�ǰ״̬��
	int add_user(int session_id, std::shared_ptr<User> user);
	int remove_user(int session_id, std::shared_ptr<User> user);
	void start_game();
	int get_homeowner();

	//��ȡ��������
	const Room_property& get_Room_property()const;
	//��������ʽ��ȡ�û��б�
	const std::list<int>& get_Room_user()const;
	//��ȡ����id
	int get_id()const;
	Game_room(io_context* io);
	Game_room(const Room_property&& prop, io_context* io);
	static ID_generator<int> room_gen;

	//��ʼ��Ϸ�����躯��---------------------------------------------------


	//�����غϣ�����ǰ�غ��Ѿ�������㣬�������ݰ���
	bool listen();
	//������Ϣ�����е���Ϣ
	std::pair<int, std::shared_ptr<Proto_msg>>msg_pop();
	//���ݻỰ�Ż�ȡ�����Ϣ
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