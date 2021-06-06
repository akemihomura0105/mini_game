#pragma once
#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <memory>
#include <list>
#include "../general_class/Game_proto.h"
#include "../general_class/state_code.h"
#include "../general_class/User_info.h"
#include "Otp_table.h"
#include "../general_class/Tcp_connection.h"
#include "../general_class/Room_info.h"
#include "threadsafe_queue.h"
#include "Game_info.h"
#include <queue>

using namespace boost::asio;

#define ASYNC_RET void
class System :public std::enable_shared_from_this<System>
{
public:
	void run();
	System(io_context& _io, ip::tcp::endpoint& _ep);

private:
	io_context& io;
	ip::tcp::endpoint& ep;
	std::shared_ptr<ip::tcp::socket>sock;
	std::shared_ptr<Tcp_connection>conn;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	int session_id;
	int room_id;
	Room_info room_info;

	int login();
	std::shared_ptr<Proto_msg> get_msg();
	boost::system::error_code send_msg(Proto_msg msg);


	void show_room();
	int join_room(int state);
	int make_room(int state);
	void exit_room();


	enum class STATE { HALL, ROOM, GAME };
	STATE state;
	threadsafe_queue<std::string>input_que;
	void otp_room_operation();
	void otp_game_operation();
	void hall_system_run();
	void room_system_run();

	void sync_time(std::shared_ptr<Proto_msg>msg);
	void game_system_run();
	void update_room_info(std::shared_ptr<Proto_msg>msg);

	void read_input();

	void set_ready();
	void start_game();

	ASYNC_RET route();
	ASYNC_RET message_route();

	std::shared_ptr<basic_game_info>game_info;
	void create_game_info(std::shared_ptr<Proto_msg>msg);
	void change_location(int location);
	void attack(int game_id);
	void heal(int game_id);
	void mine();
	void receive_state_code_result(std::shared_ptr<Proto_msg>msg);
	void receive_location_info(std::shared_ptr<Proto_msg>msg);
	void receive_hp_info(std::shared_ptr<Proto_msg>msg);
};
