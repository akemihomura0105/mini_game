#pragma once
#include <memory>
#include <string>
#include <unordered_map>

class User
{
public:

	enum class STATE_ENUM { online = 0, offline, inroom, ingame };
	int load_user(std::string username, std::string password = "");
	const std::string& get_username()const;
	void set_id(int n);
	void set_room_id(int n);
	bool operator <(const User& user)const;
	bool operator == (const User& user)const;
private:
	std::string username;
	size_t id;
	size_t room_id;
	STATE_ENUM state;
};

namespace std
{
	template<>
	struct hash<User>
	{
		std::size_t operator()(const User& key)const
		{
			return hash<string>()(key.get_username());
		}
	};
}