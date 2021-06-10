#pragma once
#include<chrono>
namespace CONSTV
{
	using namespace std::chrono;
	constexpr seconds ready = 3s;
	constexpr seconds depature0 = ready + 3s;
	constexpr seconds depature1 = depature0 + 3s;
	constexpr seconds daytime = depature1 + 5s;
	constexpr seconds night = daytime + 100s;
	constexpr int turn_duration = 5;
	constexpr int initial_HP = 2;
	constexpr int MAX_HP = 7;
	constexpr int MINE_COIN = 3;

	constexpr int TH_initial_coin = 10;
	constexpr int TH_initial_bandage = 1;
	constexpr int TH_initial_armo = 5;

	constexpr int ES_initial_coin = 0;
	constexpr int ES_initial_bandage = 0;
	constexpr int ES_initial_armo = 5;

	constexpr int bidding_init_price = 3;
	constexpr int bidding_steps = 3;
	constexpr int armo_item_stage = 0 + 2;
	constexpr int bandage_item_stage = armo_item_stage + 2;
	constexpr int auction_item_num = 4;
	constexpr seconds bidding_time = 3s;
}