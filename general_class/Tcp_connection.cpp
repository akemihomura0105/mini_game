#include "Tcp_connection.h"

Tcp_connection::Tcp_connection(io_context& _io, std::shared_ptr<ip::tcp::socket> _sock,
	std::queue<std::shared_ptr<Proto_msg>>& _msg_que, int _session_id)
	:io(_io), sock(_sock), msg_que(_msg_que), session_id(_session_id)
{
	write_buf.resize(65536);
	read_buf.resize(65536);
}

//连接建立，开始处理包头。
void Tcp_connection::run()
{
	io.post(bind(&Tcp_connection::get_msg_head, shared_from_this()));
	static boost::system::error_code event_ec;
	io.post(bind(&Tcp_connection::send_event, shared_from_this(), event_ec));
}

void Tcp_connection::pause()
{
	pause_flag = true;
}

void Tcp_connection::close()
{
	auto msg = std::make_shared<Proto_msg>(1, 50000);
	boost::system::error_code ec;
	serialize_obj(msg->body, session_id);
	push_msg(msg, ec);
}

//处理head，从socket中读取一个head大小的数据，并回调body
void Tcp_connection::get_msg_head()
{
	if (pause_flag)
		return;
	auto proto_ptr = std::make_shared<Proto_msg>();
	async_read(*sock,
		buffer(&proto_ptr->head, sizeof(Proto_head)),
		bind(&Tcp_connection::get_msg_body, shared_from_this(), proto_ptr, placeholders::error));
}

//处理body，判断魔数校验，并将最后的结果交由push_msg函数。
void Tcp_connection::get_msg_body(std::shared_ptr<Proto_msg> proto_ptr, const boost::system::error_code& ec)
{
	if (socket_error_solve(ec))
		return;
	if (!proto_ptr->head.magic != PROTO_MAGIC)
	{
		//do something
	}
	if (proto_ptr->head.service == 6)
		std::cerr << proto_ptr->head.len << "-------------------------------\n";
	proto_ptr->body.resize(proto_ptr->head.len);
	async_read(*sock,
		buffer(proto_ptr->body, proto_ptr->head.len),
		bind(&Tcp_connection::push_msg, shared_from_this(), proto_ptr, placeholders::error));
}

//将处理成功的包发送至路由缓冲区
void Tcp_connection::push_msg(std::shared_ptr<Proto_msg> proto_ptr, const boost::system::error_code& ec)
{
	if (socket_error_solve(ec))
		return;
	std::cerr << "service_id: " << proto_ptr->head.service << "\tlen：" << proto_ptr->head.len << std::endl;
	msg_que.push(proto_ptr);
	get_msg_head();
}

void Tcp_connection::push_event(std::shared_ptr<Proto_msg> msg_ptr)
{
	std::cerr << "push a event: service_id: " << msg_ptr->head.service << "\tlen: " << msg_ptr->head.len << std::endl;
	event_que.push(msg_ptr);
}

ASYNC_RET Tcp_connection::send_event(const boost::system::error_code& ec)
{
	if (pause_flag)
		return;	
	if (socket_error_solve(ec))
		return;
	if (!event_que.empty())
	{
		auto reply_msg = event_que.front();
		event_que.pop();
		reply_msg->encode(write_buf);
		std::cerr << "send_event:\n\tservice_id: " << reply_msg->head.service << "\n\tbody: " << reply_msg->body << std::endl;
		async_write(*sock, buffer(write_buf, reply_msg->head.len + sizeof(Proto_head)), bind(&Tcp_connection::send_event, shared_from_this(), placeholders::error));
	}
	else
		io.post(bind(&Tcp_connection::send_event, shared_from_this(), ec));
}

ASYNC_RET Tcp_connection::send_msg(std::shared_ptr<Proto_msg> msg_ptr)
{
	msg_ptr->encode(write_buf);
	std::cerr << msg_ptr->head.len << std::endl;
	async_write(*sock, buffer(write_buf, msg_ptr->head.len + sizeof(Proto_head)), bind(&Tcp_connection::socket_error_handle, shared_from_this(), placeholders::error));
}

bool Tcp_connection::socket_error_solve(const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message();
		pause();
		sock->close();
		close();
		return true;
	}
	return false;
}

void Tcp_connection::socket_error_handle(const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message();
		sock->close();
		close();
	}
}
