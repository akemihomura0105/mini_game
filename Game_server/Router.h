#pragma once
#include "../proto/Game_proto.h"
#include <memory>
class Router
{
public:
	static Router& get_instance();
	Router(const Router&) = delete;
	Router operator=(const Router&) = delete;
	int push_package(std::shared_ptr<Proto_msg>msg);
private:
	Router() {};
};

