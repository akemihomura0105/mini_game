#pragma once
#include "User.h"
#include "ID_generator.h"
#include "Character.h"
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
class Game_room
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
			ar& size_t(*id);
			ar& size;
			ar& capacity;
		}
		//������
		std::string name;
		//����id
		std::shared_ptr<ID<size_t>> id = nullptr;
		//���䵱ǰ�û���
		size_t size;
		//��������
		size_t capacity;
		Room_property(bool init = false);
	};

	//����session_id����Userָ�룬���û�����������ڣ����޸��û��ĵ�ǰ״̬��
	int add_user(size_t session_id, std::shared_ptr<User> user);
	int remove_user(size_t session_id, std::shared_ptr<User> user);
	int start_game();
	size_t get_homeowner();

	//��ȡ��������
	const Room_property& get_Room_property()const;
	//��������ʽ��ȡ�û��б�
	const std::list<size_t>& get_Room_user()const;
	//��ȡ����id
	size_t get_id()const;
	Game_room();
	Game_room(const Room_property&& prop);
	static ID_generator<size_t> room_gen;

	//��ʼ��Ϸ�����躯��---------------------------------------------------
	// 

	//�����غϣ�����ǰ�غ��Ѿ�������㣬�������ݰ���
	bool listen_turn();
	//������Ϣ�����е���Ϣ
	std::shared_ptr<Proto_msg>msg_pop();

private:
	Room_property prop;
	std::list<size_t>users;
	size_t rome_owner = 0;

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
	std::unordered_map<size_t, std::shared_ptr<Actionable_character>>player;
	//msg que, the system will read from this queue to send message to client.
	std::queue<Proto_msg>msg_que;
	//heal tuple, define the heal action.@1 means src's session_id, @2means des's session_id.
	typedef std::tuple<size_t, size_t>heal_tuple;
	std::queue<heal_tuple>heal_que;
	//attack tuple, define the attack action.@1 means src's session_id, @2means des's session_id.
	typedef std::tuple<size_t, size_t>atk_tuple;
	std::queue<atk_tuple>atk_que;
	std::queue<size_t>mine_que;
	//move tuple, define the move action. @1 means src's session_id, @2means des's location id.
	typedef std::tuple<size_t, int>move_tuple;
	std::queue<move_tuple>move_que;

	//Mapping from the location to the character.
	std::vector<std::unordered_set<size_t>>location;
	//Game start time.
	std::chrono::time_point<std::chrono::steady_clock>start_time;
	//End time of the last turn.
	std::chrono::time_point<std::chrono::steady_clock>last_time;

	//convert the list form to the unordered_map form.
	void load_player();
	void route(std::shared_ptr<Proto_msg>msg);
};