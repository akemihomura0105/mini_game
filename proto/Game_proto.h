/*
* Game_proto���Զ���Ӧ�ò�Э��
* 
* ����ʹ��std::string��Ϊ���ݻ�������
* 
* ���л���
* ���л�ʹ��boost::serialization��
* ����������Ҫ���л����࣬�䶼Ӧ��boost�ĵ���ָ����һ�������Լ��Ľӿڱ�¶��boost����ָ��Ҫ���л������ݡ�
* �����
* friend class boost::serialization::access
* template<typename Archive>
* void serialize(Archive& ar, const unsigned int version)
* 
* Э��ͷ���干64λ������8λ�汾�ţ�8λУ��ħ����16λ����id��32λ������
* ͨ����Э������Ϊ������������serialize_obj�����������л���Э���塣
* ��Ҫ�������ݰ���ͨ������Proto_msg::encode���ɽ����ݰ�ת�����ֽ��������벢����std::string��
* ���ݰ��Ľ��ܶ������ļ�Tcp_connection.h�У���Ҫ�ֶ������ͷ����ɰ�ͷ�����������д��䡣
* decode�������޾�������
*/


#pragma once
#include <cstdint>
#include <boost/asio.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/endian.hpp>
#include <vector>
#include <string_view>

using namespace boost::asio;

/*
* Desc:	�������л���Ҫ������������boost���л����е����л�������
* P0:	��ʾҪ���л��Ķ���
* P1:	��ʾ���л���ĳ��string���������������Զ��Ի��������ݣ����䲢����ִ��shrink������
* P2:	��ʾ�Ƿ�Ҫ׷�ӻ����� !NTT!
*/
template<typename T>
void serialize_obj(const T& obj, std::string& buffer, bool append = false)
{
	std::stringstream os;
	boost::archive::text_oarchive oa(os);
	oa << obj;
	size_t add = (append ? 0 : buffer.size());//add��ʾ��Ҫ��д��beginλ��
	buffer.resize(os.str().size() + add);
	for (int i = add; i < add + os.str().size(); i++)
		os.get(buffer[i]);
}

/*
* Desc:	�������л���Ҫ������Ϸ�
* P0:	��ʾ�������л��ֽ����Ļ�����
* P1:	��ʾ�����л��󸲸ǵĶ���
*/
template<typename T>
void deserialize_obj(const std::string& data, T& obj)
{
	std::stringstream is(data);
	boost::archive::text_iarchive ia(is);
	ia >> obj;
}

/*
* Desc:	������ʹ��boost::endian::big_uint8_tʵ��
* Tag:	serializable
* 
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
	boost::endian::big_uint8_t version;//Э��汾
	boost::endian::big_uint8_t magic;//����У��ħ��
	boost::endian::big_uint16_t service;//����id
	boost::endian::big_uint32_t len;//���峤��
	Proto_head();
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
	Proto_head head;//��ͷ
	std::string body;//����
	std::string encode();
	int decode(const std::string& data);
	Proto_msg();
	Proto_msg(boost::endian::big_uint8_t _version, boost::endian::big_uint16_t _service) :head(_version, _service) {}
	Proto_msg(const std::string& data);
};
#pragma pack(pop)