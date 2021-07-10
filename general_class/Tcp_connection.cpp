#include "Tcp_connection.h"
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
	spawn(io, bind(&Tcp_connection::get_msg, shared_from_this(), boost::placeholders::_1));
	io.post(bind(&Tcp_connection::send_event, shared_from_this()));
}

void Tcp_connection::pause()
{
	pause_flag = true;
}

void Tcp_connection::close()
{
	auto msg = std::make_shared<Proto_msg>(1, 50000);
	serialize_obj(msg->body, session_id);
	msg_que.push(msg);
}

void Tcp_connection::get_msg(yield_context yield)
{
	while (true)
	{
		auto proto_ptr = std::make_shared<Proto_msg>();
		boost::system::error_code ec;
		async_read(*sock, buffer(&proto_ptr->head, sizeof(Proto_head)), yield[ec]);
		if (socket_error_solve(ec))
			return;
		proto_ptr->body.resize(proto_ptr->head.len);
		async_read(*sock, buffer(proto_ptr->body, proto_ptr->head.len), yield[ec]);
		if (socket_error_solve(ec))
			return;
		msg_que.push(proto_ptr);
	}
}

void Tcp_connection::push_event(std::shared_ptr<Proto_msg> msg_ptr)
{
	std::cerr << "push a event: service_id: " << msg_ptr->head.service << "\tlen: " << msg_ptr->head.len << std::endl;
	event_que.push(msg_ptr);
}

void Tcp_connection::send_event()
{
	if (!event_que.empty())
	{
		auto self = shared_from_this();
		spawn(io, [this, self](yield_context yield) {
			boost::system::error_code ec;
			auto reply_msg = event_que.front();
			event_que.pop();
			reply_msg->encode(write_buf);
			std::cerr << "send_event:\n\tservice_id: " << reply_msg->head.service << "\n\tbody: " << reply_msg->body << std::endl;
			async_write(*sock, buffer(write_buf, reply_msg->head.len + sizeof(Proto_head)), yield[ec]);
			if (socket_error_solve(ec))
				return;
			});
	}
	io.post(bind(&Tcp_connection::send_event, shared_from_this()));
}

bool Tcp_connection::socket_error_solve(const boost::system::error_code& ec)
{
	if (ec)
	{
		if (ec == error::connection_reset || ec == error::eof)
		{
			auto msg = std::make_shared<Proto_msg>(1, 50000);
			serialize_obj(msg->body, session_id);
			msg_que.push(msg);
		}
		std::cerr << ec.message();
		return true;
	}
	return false;
}