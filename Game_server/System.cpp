#include "System.h"

System::System(io_context& _io, ip::tcp::endpoint& _ep) :io(_io), ep(_ep), acp(_io, _ep)
{
}

void System::run()
{
	auto ec_ptr = std::make_shared<boost::system::error_code>();
	io.post(bind(&System::accept_handler, shared_from_this(), nullptr, *ec_ptr));
	io.post(bind(&System::route, shared_from_this()));
}

void System::accept_handler(std::shared_ptr<ip::tcp::socket>sock, const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message();
	}
	if (sock)
	{
		std::shared_ptr<Tcp_connection>conn;
		size_t session_id = 0;
		if (session_gc.empty())
			session_id = session.size();
		else
		{
			session_id = session_gc.top();
			session_gc.pop();
		}
		conn = std::make_shared<Tcp_connection>(io, sock, msg_que, session_id);
		if (session.size() <= session_id)
			session.push_back(conn);
		else
			session[session_id] = conn;

		io.post(bind(&Tcp_connection::run, conn));
		auto msg = std::make_shared<Proto_msg>(1, 1);
		serialize_obj(msg->body, session_id);
		io.post(bind(&Tcp_connection::send_msg, conn, msg));
	}
	std::cerr << "成功运行accept_handle\n";
	auto new_sock = std::make_shared<ip::tcp::socket>(io);
	acp.async_accept(*new_sock, boost::bind(&System::accept_handler, shared_from_this(), new_sock, placeholders::error));
}

ASYNC_RET System::login(std::shared_ptr<Proto_msg>msg)
{
	int session_id;
	std::string username, password;
	deserialize_obj(msg->body, session_id, username);
	User user;
	auto login_msg = std::make_shared<Proto_msg>(1, 1);
	state_code sc;
	if (user_list.find(user) != user_list.end())
	{
		sc.set(CODE::LOGIN_REPEATED);
		serialize_obj(login_msg->body, sc);
		std::cout << "login failed\n";
	}
	else
	{
		user.load_user(username, password);
		user_list.insert(user);
		sc.set(CODE::LOGIN_SUCCESS);
		serialize_obj(login_msg->body, sc);
		std::string str;
		serialize_obj(str, std::string("ss"));
		std::cout << "login success\n";
	}
	io.post(bind(&Tcp_connection::send_msg, session[session_id], login_msg));
}

ASYNC_RET System::route()
{
	if (msg_que.empty())
	{
		io.post(bind(&System::route, shared_from_this()));
		return;
	}
	std::shared_ptr<Proto_msg> msg = msg_que.front();
	msg_que.pop();
	std::cout << msg->head.service << std::endl;
	switch (msg->head.service)
	{
		//case 1:登录服务
	case 1:
	{
		login(msg);
		break;
	}
	}

	io.post(bind(&System::route, shared_from_this()));
}
