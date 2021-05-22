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
	std::cout << "包体大小：" << proto_ptr->head.len << std::endl;
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
	route(proto_ptr);
	//Router::get_instance().push_package(shared_from_this(), proto_ptr);
	get_msg_head();
}

void Tcp_connection::send_event()
{
	if (event_que.empty())
	{
		auto reply_msg = std::make_shared<Proto_msg>(1);
		reply_msg->head.service = 0;
		//async_write(*sock, buffer(reply_msg->encode()), bind(Tcp_connection::socket_error_handle, placeholders::error));
	}
	else
	{
		auto reply_msg = event_que.front();
		event_que.pop();
		//async_write(*sock, buffer(reply_msg->encode()), bind(Tcp_connection::socket_error_handle, placeholders::error));
	}
}

ASYNC_RET Tcp_connection::send_msg(std::shared_ptr<ip::tcp::socket>sock, std::shared_ptr<Proto_msg> msg_ptr)
{
	static char write_buf[1024];
	auto ptr = msg_ptr->encode();
	async_write(*sock, buffer(*ptr), bind(Tcp_connection::socket_error_handle, ptr, placeholders::error));
}

void Tcp_connection::route(std::shared_ptr<Proto_msg> msg_ptr)
{
	static int cnt = 0;
	std::cout << msg_ptr->head.service << std::endl;
	switch (msg_ptr->head.service)
	{
		//case 1:登录服务
	case 1:
	{
		std::string username;
		deserialize_obj(msg_ptr->body, username);
		System::get_instance().login(sock, username);
		break;
	}
	}
}

void Tcp_connection::socket_error_handle(std::shared_ptr<std::string>buf, const boost::system::error_code& ec)
{
	if (ec)
		std::cerr << ec.message();
}
