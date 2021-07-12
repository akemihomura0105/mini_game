#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

struct Resource
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& coin;
		ar& armo;
		ar& bandage;
		ar& hint;
	}
	int coin = 0;
	int armo = 0;
	int bandage = 0;
	int hint = 0;
	Resource() {}
	Resource(int _coin, int _armo, int _bandage) :coin(_coin), armo(_armo), bandage(_bandage) {}
	Resource operator/(int k)const
	{
		Resource res;
		res.coin = coin / k;
		res.armo = armo / k;
		res.bandage = bandage / k;
		res.hint = hint / k;
		return res;
	}
	Resource operator%(int k)const
	{
		Resource res;
		res.coin = coin % k;
		res.armo = armo % k;
		res.bandage = bandage % k;
		res.hint = hint % k;
		return res;
	}
	Resource& operator+=(Resource res)
	{
		coin += res.coin;
		armo += res.armo;
		bandage += res.bandage;
		hint += res.hint;
		return *this;
	}
	std::vector<Resource> divide(int k)
	{
		std::vector<Resource>ans;
		ans.resize(k, *this / k);
		ans.back() += *this % k;
		std::shuffle(ans.begin(), ans.end(), std::default_random_engine(time(0)));
		return ans;
	}
	void clear()
	{
		coin = 0;
		armo = 0;
		bandage = 0;
		hint = 0;
	}
};