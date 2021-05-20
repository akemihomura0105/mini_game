#pragma once
class Router
{
public:
	static Router& get_instance();
	Router(const Router&) = delete;
	Router operator=(const Router&) = delete;
private:
	Router() {};
};

