#pragma once
#pragma once
#include<string>
#include<boost/serialization/serialization.hpp>
enum  CODE {
	NONE,
	LOGIN_SUCCESS,
	LOGIN_REPEATED,
	ROOM_FULL,
	ROOM_NOT_EXIST,
	NOT_HOME_OWNER,
	ROOM_NOT_FULL,
	NOT_READY,
	START_GAME,
	NO_ACTION,
	ALREADY_DEAD,
	DIFFERENT_PLACE,
	ATTACK_SUCCESS,
	NO_ARMO,
	NO_BANDAGE,
	OBJECT_HAS_DEAD,
	MOVE_SUCCESS,
	MOVE_TO_SAME_LOCATION,
	NOT_YOUR_MOVE_TURN,
	MOVE_TO_THE_BASE,
	SKILL_STILL_IN_COOLDOWN,
	HEAL_SUCCESS,
	OBJECT_HAS_FULL_HP,
	MINE_SUCCESS,
	NO_ENOUGH_MONEY,
	BID_SUCCESS,
	BUY_SUCCESS,
	EXPLORE_RECEIVE,
	EXPLORE_SUCCESS,
	EXPLORE_FAILED
};
/*namespace boost
{
	namespace serialization
	{
		template<typename Archive>
		void serialize(Archive& ar, CODE& a, const unsigned int version)
		{
			ar& a;
		}
	}
}*/

class state_code
{
public:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& code;
	}
	void set(CODE code);
	const std::string& message();
	operator int()const;
	bool operator==(const state_code& sc)const;
	bool operator==(const CODE code)const;
	bool operator!=(const CODE code)const;
private:
	CODE code = CODE::NONE;
	static std::string message_def[1000];
};