#include "Router.h"
#include <iostream>

Router& Router::get_instance()
{
	static Router router;
	return router;
}

int Router::push_package(std::shared_ptr<Proto_msg> msg)
{
	switch (msg->head.service)
	{
		//case 1:µÇÂ¼·şÎñ
	case 1:
	{
		std::string username;
		deserialize_obj(msg->body, username);
		std::cout << username;
	}
	}
	return 0;
}
