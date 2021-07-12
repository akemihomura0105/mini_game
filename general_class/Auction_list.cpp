#include "Auction_list.h"

const std::array<std::string, 2> Auction_item::item_name = { "±Á´ø","×Óµ¯" };

void Auction_item::reset(int _item_id)
{
	item_id = _item_id;
	bidder = -1;
	switch (item_id)
	{
	default:
		break;
	case 0:
		price = CONSTV::bidding_init_price;
		break;
	case 1:
		price = CONSTV::bidding_init_price;
		break;
	}
}
