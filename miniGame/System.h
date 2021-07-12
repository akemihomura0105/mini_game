#pragma once
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
#include "../general_class/Auction_list.h"
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
	ip::tcp::endpoint ep;
	std::shared_ptr<ip::tcp::socket>sock;
	std::shared_ptr<Tcp_connection>conn;
	std::shared_ptr<Tcp_connection>room_conn;
	std::shared_ptr<ip::tcp::socket>room_sock;
	std::queue<std::shared_ptr<Proto_msg>>msg_que;
	int session_id;
	int room_id;
	Room_info room_info;
	std::chrono::time_point<std::chrono::steady_clock>start_time;

	void receive_session_id(std::shared_ptr<Proto_msg>msg);
	void login(std::string_view username);
	void request_room_prop(bool no_cycle = false);
	void receive_room_prop(std::shared_ptr<Proto_msg>msg);
	void join_room(int room_id);
	void receive_join_result(std::shared_ptr<Proto_msg>msg);
	void make_room(std::string_view user_input);
	void receive_make_result(std::shared_ptr<Proto_msg>msg);
	void exit_room();


	enum class STATE { LOGIN, WAIT_LOGIN, WAIT_JOIN, HALL, ROOM, GAME };
	STATE state;
	threadsafe_queue<std::string>input_que;
	void otp_hall_operation();
	void otp_room_operation();
	void otp_game_operation();
	void hall_system_run();
	void room_system_run();

	void sync_time(std::shared_ptr<Proto_msg>msg);
	void game_system_run(std::shared_ptr<Proto_msg>msg, yield_context yield);
	void update_room_info(std::shared_ptr<Proto_msg>msg);

	void read_input();

	void set_ready();
	void start_game();

	void route();
	void message_route();

	std::shared_ptr<basic_game_info>game_info;
	void create_game_info(std::shared_ptr<Proto_msg>msg);
	bool daytime_action_check();
	void change_location(int location);
	void attack(int game_id);
	void heal(int game_id);
	void mine();
	void explore();
	void bid(int price);
	void receive_state_code_result(std::shared_ptr<Proto_msg>msg);
	void receive_location_info(std::shared_ptr<Proto_msg>msg);
	void receive_ghost_sight(std::shared_ptr<Proto_msg>msg);
	void receive_hp_info(std::shared_ptr<Proto_msg>msg);
	void receive_res_info(std::shared_ptr<Proto_msg>msg);
	void receive_treasure_result(std::shared_ptr<Proto_msg>msg);
	void receive_treasure_info(std::shared_ptr<Proto_msg>msg);
	void receive_bid_info(std::shared_ptr<Proto_msg>msg);
	void receive_buyer_info(std::shared_ptr<Proto_msg>msg);
	void receive_stage_change(std::shared_ptr<Proto_msg>msg);
	void game_finish(std::shared_ptr<Proto_msg>msg);
};