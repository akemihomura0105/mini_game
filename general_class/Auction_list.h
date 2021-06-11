#pragma once
#include <vector>
#include <array>
#include <string>
#include "game_const_value.h"

struct Auction_item
{
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& item_id;
		ar& price;
		ar& bidder;
	}
	const static std::array<std::string, 2>item_name;
	int item_id = 0;
	//item_id
	// 	   0: bandage
	// 	   1: armo
	//
	int price;
	int bidder;

	void reset(int item_id);
};
//class Auction_list
//{
//public:
//	template<typename Archive>
//	void serialize(Archive& ar, const unsigned int version)
//	{
//		ar& item;
//	}
//	bool bid(int item_id, int price);
//
//	void init_list();
//
//	const std::vector<Auction_item>& get_list();
//
//	Auction_list(int n = 2);
//private:
//	std::vector<Auction_item>item;
//};