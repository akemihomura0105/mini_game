#pragma once
#include <memory>
#include <string>
#include <unordered_map>
class User
{
public:
	enum class STATE_ENUM { online = 0, offline, inroom, ingame };
	int load_user(std::string username, std::string password = "");
	void set_id(int n);
	bool operator <(const User& user)const;
private:
	std::string username;
	size_t id;
	STATE_ENUM state;
};

