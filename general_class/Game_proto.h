/*
* Game_proto，自定义应用层协议
*
* 代码使用std::string作为数据缓冲区。
*
* 序列化：
* 序列化使用boost::serialization。
* 对于所有需要序列化的类，其都应如boost文档中指出的一样，将自己的接口暴露给boost，并指定要序列化的内容。
* 即添加
* friend class boost::serialization::access
* template<typename Archive>
* void serialize(Archive& ar, const unsigned int version)
*
* 协议头定义共64位，包括8位版本号，8位校验魔数，16位服务id，32位包长度
* 通过将协议体作为缓冲区，调用serialize_obj，将对象序列化至协议体。
* 若要发送数据包：通过调用Proto_msg::encode，可将数据包转换成字节流，存入并返回std::string。
* 数据包的接受定义在文件Tcp_connection.h中，需要手动读入包头，完成包头解析，并进行传输。
*/

#pragma once
#include <cstdint>
#include <boost/asio.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/endian.hpp>
#include <vector>
#include "../general_class/state_code.h"

using namespace boost::asio;

template<typename T>
void deserialize_obj0(boost::archive::text_iarchive& ia, T& obj)
{
	ia >> obj;
}
template<typename T, typename ...Args>
void deserialize_obj0(boost::archive::text_iarchive& ia, T& obj, Args&... args)
{
	ia >> obj;
	deserialize_obj0(ia, args...);
}
/*
* Desc:	对象反序列化，要求输入合法
* data:	表示经过序列化字节流的缓冲区
* args:	表示反序列化后覆盖的对象。
*/
template<typename ...Args>
void deserialize_obj(const std::string& data, Args&... args)
{
	std::stringstream is(data);
	boost::archive::text_iarchive ia(is);
	deserialize_obj0(ia, args...);
}

template<typename T>
void serialize_obj0(boost::archive::text_oarchive& oa, std::stringstream& os, std::string& buffer, T& obj)
{
	oa << obj;
	buffer.resize(os.str().size());
	for (int i = 0; i < os.str().size(); i++)
		os.get(buffer[i]);
}
template<typename T, typename ...Args>
void serialize_obj0(boost::archive::text_oarchive& oa, std::stringstream& os, std::string& buffer, T& obj, Args&&... args)
{
	oa << obj;
	serialize_obj0(oa, os, buffer, args...);
}
/*
* Desc:		对象序列化，要求对象必须满足boost序列化库中的序列化条件。
* buffer:	表示序列化至某个string缓冲区，函数将自动对缓冲区扩容，但其并不会执行shrink操作。
* args:		表示要序列化的对象列表。
*/
template<typename ...Args>
void serialize_obj(std::string& buffer, Args&&... args)
{
	std::stringstream os;
	boost::archive::text_oarchive oa(os);
	buffer.clear();
	serialize_obj0(oa, os, buffer, std::forward<Args&>(args)...);
}

/*
* Desc:	网络序使用boost::endian::big_uint8_t实现
* Tag:	serializable
*/
#pragma pack(push,4)
const boost::endian::big_uint8_t PROTO_MAGIC = 0x12;
struct Proto_head
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& version;
		ar& magic;
		ar& service;
		ar& len;
	}
	boost::endian::big_uint8_t version;//协议版本
	boost::endian::big_uint8_t magic;//快速校验魔数
	boost::endian::big_uint16_t service;//服务id
	boost::endian::big_uint32_t len;//包体长度
	Proto_head();
	Proto_head(uint16_t _version);
	Proto_head(uint16_t _version, uint16_t _service);
};

struct Proto_msg
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& head;
		ar& body;
	}
	Proto_head head;//包头
	std::string body;//包体
	std::shared_ptr<std::string> encode();
	void encode(std::string& buffer);
	int decode(const std::string& data);
	Proto_msg();
	Proto_msg(boost::endian::big_uint8_t _version) :head(_version) {}
	Proto_msg(boost::endian::big_uint8_t _version, boost::endian::big_uint16_t _service) :head(_version, _service) {}
	Proto_msg(const std::string& data);
};
#pragma pack(pop)