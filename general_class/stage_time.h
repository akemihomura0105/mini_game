#pragma once
#include<chrono>
namespace stage_time
{
	using namespace std::chrono;
	constexpr seconds ready = 5s;
	constexpr seconds depature0 = ready + 10s;
	constexpr seconds depature1 = depature0 + 10s;
}