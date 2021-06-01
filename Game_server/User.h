#pragma once
#include <memory>
#include <string>
#include <unordered_map>

class User
{
public:

	enum class STATE_ENUM { online = 0, offline, inroom, ingame };
	int load_user(const std::string& username, const std::string& password = "");
	const std::string& get_username()const;
	void set_id(int n);
	void set_room_id(int room_id);
	int get_room_id();
	bool operator <(const User& user)const;
	bool operator == (const User& user)const;
	bool is_ready()const;
	void set_ready();
private:
	std::string username;
	int id;
	int room_id;
	bool ready = false;
	STATE_ENUM state;
};

namespace std
{
	template<>
	struct hash<User>
	{
		int operator()(const User& key)const
		{
			return hash<string>()(key.get_username());
		}
	};
}