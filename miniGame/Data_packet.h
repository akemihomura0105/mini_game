#pragma once
#include <cstdint>
#include <boost/asio.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <string_view>


using namespace boost::asio;

template<typename T>
std::string serialize(const T& obj)
{
	std::stringstream os;
	boost::archive::text_oarchive oa(os);
	oa << obj;
	return os
}

template<typename T>
void deserialize(T& obj)
{

}

class Proto_head
{
	uint16_t version;//协议版本
	uint16_t service;//服务id
	uint32_t len;//body长度
};

class Proto_msg
{
	Proto_head head;
	uint8_t* body;
	uint8_t* encode();
	void decode(uint8_t*);
};




class Login_msg :public Proto_msg
{

	Login_msg(const Proto_head& _head, std::string_view username);
};