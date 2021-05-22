#include "Router.h"
#include "System.h"
#include <iostream>

Router& Router::get_instance()
{
	static Router router;
	return router;
}

int Router::push_package(std::shared_ptr<Tcp_connection>conn, std::shared_ptr<Proto_msg> msg)
{
	switch (msg->head.service)
	{
		//case 1:��¼����
	case 1:
	{
		std::string username;
		deserialize_obj(msg->body, username);
		System::get_instance().login(conn, username);
	}
	}
	return 0;
}
