#include "Tcp_connection.h"

Tcp_connection::Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket> _sock) :io(_io), sock(_sock)
{
}

//���ӽ�������ʼ�����ͷ��
void Tcp_connection::run()
{
	get_msg_head();
}

//����head����socket�ж�ȡһ��head��С�����ݣ����ص�body
void Tcp_connection::get_msg_head()
{
	auto proto_ptr = std::make_shared<Proto_msg>();
	/*read(*sock, buffer(&proto_ptr->head, sizeof(Proto_head)));
	for (int i = 0; i < sizeof(Proto_head); i++)
		std::cout << (int)*((char*)&proto_ptr->head + i) << " ";
	std::cout << std::endl << proto_ptr->head.len;*/
	async_read(*sock,
		buffer(&proto_ptr->head, sizeof(Proto_head)),
		bind(&Tcp_connection::get_msg_body, shared_from_this(), proto_ptr, placeholders::error));
}

//����body���ж�ħ��У�飬�������Ľ������push_msg������
void Tcp_connection::get_msg_body(std::shared_ptr<Proto_msg> proto_ptr, const boost::system::error_code& ec)
{
	std::cout << proto_ptr->head.len;
	if (ec)
	{
		std::cerr << ec.message();
	}
	if (!proto_ptr->head.magic != PROTO_MAGIC)
	{
		//do something
	}
	proto_ptr->body.resize(proto_ptr->head.len);
	async_read(*sock,
		buffer(proto_ptr->body, proto_ptr->head.len),
		bind(&Tcp_connection::push_msg, shared_from_this(), proto_ptr, placeholders::error));
}

//������ɹ��İ�������·�ɻ�����
void Tcp_connection::push_msg(std::shared_ptr<Proto_msg> proto_ptr, const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message();
	}
	Router::get_instance().push_package(proto_ptr);
	get_msg_head();
}