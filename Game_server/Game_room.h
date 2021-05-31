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
		//房间名
		std::string name;
		//房间id
		std::shared_ptr<ID<size_t>> id = nullptr;
		//房间当前用户数
		size_t size;
		//房间容量
		size_t capacity;
		Room_property(bool init = false);
	};

	//传入session_id，和User指针，将用户添加至房间内，并修改用户的当前状态。
	int add_user(size_t session_id, std::shared_ptr<User> user);
	int remove_user(size_t session_id, std::shared_ptr<User> user);
	int start_game();
	size_t get_homeowner();

	//获取房间属性
	const Room_property& get_Room_property()const;
	//以链表形式获取用户列表
	const std::list<size_t>& get_Room_user()const;
	//获取房间id
	size_t get_id()const;
	Game_room();
	Game_room(const Room_property&& prop);
	static ID_generator<size_t> room_gen;

	//开始游戏后所需函数---------------------------------------------------
	// 

	//监听回合，若当前回合已经进入结算，则处理数据包。
	bool listen_turn();
	//弹出消息队列中的消息
	std::shared_ptr<Proto_msg>msg_pop();

private:
	Room_property prop;
	std::list<size_t>users;
	size_t rome_owner = 0;

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