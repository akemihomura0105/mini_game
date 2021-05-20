#pragma once
#include <cstdint>
#include <boost/asio.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <vector>
#include <string_view>


using namespace boost::asio;

template<typename T>
void serialize(const T& obj, std::vector<char>& buffer, bool append = false)
{
	std::stringstream os;
	boost::archive::text_oarchive oa(os);
	oa << obj;
	if (append)
	{
		buffer.reserve(buffer.size() + os.str().size());
		for (auto c : os.str())
			buffer.push_back(c);
	}
	else
		buffer.assign(os.str().begin(), os.str().end());
}

template<typename T>
void deserialize(char* data, size_t len, T& obj)
{
	std::stringstream is;
	boost::archive::text_iarchive ia(is);
	ia >> obj;
}

struct Proto_head
{
	uint16_t version;//Э��汾
	uint16_t service;//����id
	uint32_t len;//body����
	Proto_head(uint16_t _version, uint16_t _service) {}
};

struct Proto_msg
{
	Proto_head head;//Э��ͷ
	std::vector<char> body;//Э����
	std::vector<char> encode_buf();
	void decode(uint8_t* data);
	Proto_msg(uint16_t _version, uint16_t _service) :head(_version, _service) {}
};

class Login_msg :public Proto_msg
{
	Login_msg(const Proto_head& _head, std::string_view username);
};