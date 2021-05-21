#include "Tcp_connection.h"

Tcp_connection::Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket> _sock) :io(_io), sock(_sock)
{
}

//连接建立，开始处理包头。
void Tcp_connection::run()
{
	get_msg_head();
}

//处理head，从socket中读取一个head大小的数据，并回调body
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

//处理body，判断魔数校验，并将最后的结果交由push_msg函数。
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

//将处理成功的包发送至路由缓冲区
void Tcp_connection::push_msg(std::shared_ptr<Proto_msg> proto_ptr, const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message();
	}
	Router::get_instance().push_package(proto_ptr);
	get_msg_head();
}