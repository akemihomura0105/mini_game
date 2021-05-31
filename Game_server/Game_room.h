#pragma once
#include "User.h"
#include "ID_generator.h"
#include "Character_factory.h"
#include "../general_class/Game_proto.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/list.hpp>
#include <set>
#include <map>
#include <list>
#include <unordered_set>
#include <tuple>
#include <random>

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
		int size;
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
	Game_room(io_context& io);
	Game_room(const Room_property&& prop, io_context& io);
	static ID_generator<int> room_gen;

	//��ʼ��Ϸ�����躯��---------------------------------------------------


	//�����غϣ�����ǰ�غ��Ѿ�������㣬�������ݰ���
	bool listen();
	//������Ϣ�����е���Ϣ
	std::pair<int, std::shared_ptr<Proto_msg>>msg_pop();
	//���ݻỰ�Ż�ȡ�����Ϣ
	std::shared_ptr<Actionable_character> get_player(int session_id);
private:

	io_context& io;
	bool listen_flag = false;
	Room_property prop;
	std::list<int>users;
	int rome_owner = 0;
	enum class STAGE { READY, DAYTIME, NIGHT };
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
	//event que, the system will read from this queue to send message to client.
	std::queue<std::pair<int, std::shared_ptr<Proto_msg>>>msg_que;
	//heal tuple, define the heal action.@1 means src's session_id, @2means des's session_id.
	typedef std::tuple<int, int>heal_tuple;
	std::queue<heal_tuple>heal_que;
	//attack tuple, define the attack action.@1 means src's session_id, @2means des's session_id.
	typedef std::tuple<int, int>atk_tuple;
	std::queue<atk_tuple>atk_que;
	std::queue<int>mine_que;
	//move tuple, define the move action. @1 means src's session_id, @2means des's location id.
	typedef std::tuple<int, int>move_tuple;
	std::queue<move_tuple>move_que;

	//Mapping from the location to the character.
	std::vector<std::unordered_set<int>>location;
	//Game start time.
	std::chrono::time_point<std::chrono::steady_clock>start_time;
	//End time of the last turn.
	std::chrono::time_point<std::chrono::steady_clock>last_time;

	//convert the list form to the unordered_map form.
	void load_player();
	std::chrono::seconds get_duration_since_last_stage();
	bool switch_stage(std::chrono::seconds sec);
	void ready_stage(bool exec = true);
	void depature_stage0(bool exec = true);
	void depature_stage1(bool exec = true);
	void broadcast_character();
	void broadcast_location(bool all_flag = false);
	void broadcast_hp(bool all_flag = false);
};