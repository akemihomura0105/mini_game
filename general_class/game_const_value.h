#pragma once
#include<chrono>
enum class STAGE { READY, DEPATURE0, DEPATURE1, DAYTIME, NIGHT0, NIGHT1 };
namespace CONSTV
{
	using namespace std::chrono;
	constexpr seconds ready = 1s;
	constexpr seconds depature0 = ready + 2s;
	constexpr seconds depature1 = depature0 + 2s;
	constexpr int day_turn = 3;
	constexpr seconds turn_duration = 5s;
	constexpr seconds daytime = depature1 + day_turn * turn_duration;
	constexpr seconds night0 = daytime + 5s;
	constexpr seconds bidding_time = 5s;
	constexpr seconds night1 = night0 + 100s;
	constexpr int initial_HP = 2;
	constexpr int MAX_HP = 7;
	constexpr int MINE_COIN = 3;

	constexpr int TH_initial_coin = 10;
	constexpr int TH_initial_bandage = 1;
	constexpr int TH_initial_armo = 5;
	constexpr double explore_probability = 1.0;

	constexpr int ES_initial_coin = 0;
	constexpr int ES_initial_bandage = 0;
	constexpr int ES_initial_armo = 5;

	constexpr int bidding_init_price = 3;
	constexpr int bidding_steps = 3;
	constexpr int armo_item_stage = 0 + 2;
	constexpr int bandage_item_stage = armo_item_stage + 2;
	constexpr int auction_item_num = 4;
}