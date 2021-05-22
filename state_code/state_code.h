#pragma once
#include<string>
#include<boost/serialization/serialization.hpp>
enum class CODE { NONE, LOGIN_SUCCESS, LOGIN_REPEATED };
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
private:
	CODE code = CODE::NONE;
};

