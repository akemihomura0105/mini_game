#pragma once
#include<chrono>
namespace CONSTV
{
	using namespace std::chrono;
	constexpr seconds ready = 5s;
	constexpr seconds depature0 = ready + 5s;
	constexpr seconds depature1 = depature0 + 5s;
	constexpr seconds daytime = depature1 + 40s;
	constexpr seconds night = daytime + 10s;
	constexpr int turn_duration = 10;
	constexpr int initial_HP = 1;
	constexpr int MAX_HP = 7;
	constexpr int MINE_COIN = 3;

	constexpr int TH_initial_coin = 0;
	constexpr int TH_initial_bandage = 0;
	constexpr int TH_initial_armo = 5;

	constexpr int ES_initial_coin = 0;
	constexpr int ES_initial_bandage = 0;
	constexpr int ES_initial_armo = 5;
}