#include "Game_proto.h"

Proto_head::Proto_head() {}
Proto_head::Proto_head(uint16_t _version, uint16_t _service) :version(_version), service(_service) {}


constexpr int place0 = 0;
constexpr int place1 = place0 + sizeof(Proto_head::version);
constexpr int place2 = place1 + sizeof(Proto_head::service);

std::vector<char> Proto_msg::encode()
{
	std::vector<char>data;
	data.resize(sizeof(Proto_head) + body.size());
	auto net_version = detail::socket_ops::host_to_network_short(head.version);
	auto net_service = detail::socket_ops::host_to_network_short(head.service);
	auto net_len = detail::socket_ops::host_to_network_long(body.size());
	memcpy(&data[place0], &net_version, sizeof(net_version));
	memcpy(&data[place1], &net_service, sizeof(net_service));
	memcpy(&data[place2], &net_service, sizeof(net_service));
	std::copy(body.begin(), body.end(), data.begin() + sizeof(Proto_head));
	return data;
}

void Proto_msg::decode(const std::vector<char>& data)
{
	auto version_ptr = reinterpret_cast<const uint16_t*>(&data[place0]);
	auto service_ptr = reinterpret_cast<const uint16_t*>(&data[place1]);
	auto len_ptr = reinterpret_cast<const uint32_t*>(&data[place2]);
	head.version = detail::socket_ops::network_to_host_short(*version_ptr);
	head.service = detail::socket_ops::network_to_host_short(*service_ptr);
	body.resize(detail::socket_ops::network_to_host_long(*len_ptr));
	std::copy(data.begin() + sizeof(Proto_head), data.end(), body.begin());
}

Proto_msg::Proto_msg() {}

Proto_msg::Proto_msg(const std::vector<char>& data)
{
	decode(data);
}

