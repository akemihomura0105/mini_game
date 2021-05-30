#include "Game_proto.h"
#include <iostream>

Proto_head::Proto_head() { memset(this, 0, sizeof(Proto_head)); }
Proto_head::Proto_head(uint16_t _version) :version(_version) {}
Proto_head::Proto_head(uint16_t _version, uint16_t _service) : version(_version), service(_service) { len = 0; }


std::shared_ptr<std::string> Proto_msg::encode()
{
	head.len = body.size();
	auto data = std::make_shared<std::string>();
	data->resize(sizeof(Proto_head) + body.size());
	memcpy(&(*data)[0], &head, sizeof(Proto_head));
	std::copy(body.begin(), body.end(), data->begin() + sizeof(Proto_head));
	return data;
}

void Proto_msg::encode(std::string& buffer)
{
	head.len = body.size();
	assert(buffer.size() >= sizeof(Proto_head) + head.len);
	memcpy(&buffer[0], &head, sizeof(Proto_head));
	std::copy(body.begin(), body.end(), &buffer[sizeof(Proto_head)]);
}

int Proto_msg::decode(const std::string& data)
{
	return 0;
}

Proto_msg::Proto_msg() {}

Proto_msg::Proto_msg(const std::string& data)
{
	decode(data);
}