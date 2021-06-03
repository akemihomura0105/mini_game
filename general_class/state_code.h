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
	NOT_READY,
	NO_ACTION,
	ATTACK_SUCCESS,
	NO_ARMO,
	NO_BANDAGE,
	OBJECT_HAS_DEAD,
	MOVE_SUCCESS,
	MOVE_TO_SAME_LOCATION,
	SKILL_STILL_IN_COOLDOWN,
	HEAL_SUCCESS,
	OBJECT_HAS_FULL_HP,
	MINE_SUCCESS
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
	bool operator==(const state_code& sc)const;
	bool operator==(const CODE code)const;
	bool operator!=(const CODE code)const;
private:
	CODE code = CODE::NONE;
	static std::string message_def[1000];
};