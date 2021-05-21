#include "Game_proto.h"
#include <iostream>

Proto_head::Proto_head() { memset(this, 0, sizeof(Proto_head)); }
Proto_head::Proto_head(uint16_t _version, uint16_t _service) :version(_version), service(_service) {}


std::string Proto_msg::encode()
{
	head.len = body.size();
	std::string data;
	data.resize(sizeof(Proto_head) + body.size());
	memcpy(&data[0], &head, sizeof(Proto_head));
	std::copy(body.begin(), body.end(), data.begin() + sizeof(Proto_head));
	return data;
}

int Proto_msg::decode(const std::string& data)
{
	constexpr int version = 0;
	constexpr int magic = version + sizeof(Proto_head::version);
	constexpr int service = magic + sizeof(Proto_head::magic);
	constexpr int len = magic + sizeof(Proto_head::service);
	auto magic_ptr = reinterpret_cast<const boost::endian::big_uint8_t*>(&data[magic]);
	if (*magic_ptr != PROTO_MAGIC)
	{
		//do_something();
	}
	auto version_ptr = reinterpret_cast<const boost::endian::big_uint8_t*>(&data[version]);
	auto service_ptr = reinterpret_cast<const boost::endian::big_uint8_t*>(&data[service]);
	auto len_ptr = reinterpret_cast<const boost::endian::big_uint32_t*>(&data[len]);
	head.service = *service_ptr;
	head.version = *version_ptr;
	head.len = *len_ptr;
	std::copy(data.begin() + sizeof(Proto_head), data.end(), body.begin());
	return 0;
}

Proto_msg::Proto_msg() {}

Proto_msg::Proto_msg(const std::string& data)
{
	decode(data);
}
