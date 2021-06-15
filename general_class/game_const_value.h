#pragma once
#include<chrono>
enum class STAGE { READY, DEPATURE0, DEPATURE1, DAYTIME, NIGHT0, NIGHT1, END };
namespace CONSTV
{
	using namespace std::chrono;

	constexpr seconds update_room_list_duration = 10s;


	constexpr seconds ready = 5s;
	constexpr seconds depature0 = ready + 10s;
	constexpr seconds depature1 = depature0 + 10s;
	constexpr int day_turn = 4;
	constexpr seconds turn_duration = 10s;
	constexpr seconds daytime = depature1 + day_turn * turn_duration;
	constexpr seconds night0 = daytime + 10s;
	constexpr seconds bidding_time = 5s;
	constexpr seconds night1 = night0 + 100s;
	constexpr int initial_HP = 10;
	constexpr int MAX_HP = 10;
	static_assert(MAX_HP >= initial_HP, "最大hp小于初始hp");
	constexpr int MINE_COIN = 3;
	constexpr int HINT_GOAL = 3;

	constexpr int TH_initial_coin = 10;
	constexpr int TH_initial_bandage = 1;
	constexpr int TH_initial_armo = 5;
	constexpr double explore_probability = 0.2;

	constexpr int ES_initial_coin = 0;
	constexpr int ES_initial_bandage = 0;
	constexpr int ES_initial_armo = 5;
	constexpr int ES_initial_skill_charge = 1;
	constexpr int ES_MAX_charge_num = 1;
	constexpr int ES_skill_cooldown_time = 3;

	constexpr int bidding_init_price = 3;
	constexpr int bidding_steps = 5;
	constexpr int armo_item_stage = 0 + 2;
	constexpr int bandage_item_stage = armo_item_stage + 2;
	constexpr int auction_item_num = 4;
	constexpr int MAX_BID_PRICE = 999;
	static_assert(auction_item_num == bandage_item_stage, "拍卖物品与回合数不匹配");
}