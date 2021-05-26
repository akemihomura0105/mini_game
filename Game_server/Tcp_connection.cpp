#include "Tcp_connection.h"

Tcp_connection::Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket> _sock,
	std::queue<std::shared_ptr<Proto_msg>>& _msg_que, std::shared_ptr<ID<size_t>> _session_id)
	:io(_io), sock(_sock), msg_que(_msg_que), session_id(_session_id)
{
	write_buf.resize(65536);
	read_buf.resize(65536);
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
	msg_que.push(proto_ptr);
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

ASYNC_RET Tcp_connection::send_msg(std::shared_ptr<Proto_msg> msg_ptr)
{
	msg_ptr->encode(write_buf);
	std::cerr << msg_ptr->head.len << std::endl;
	async_write(*sock, buffer(write_buf, msg_ptr->head.len + sizeof(Proto_head)), bind(&Tcp_connection::socket_error_handle, shared_from_this(), placeholders::error));
}

ASYNC_RET Tcp_connection::socket_error_handle(const boost::system::error_code& ec)
{
	if (ec)
		std::cerr << ec.message();
}
