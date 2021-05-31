#include "Character_factory.h"

std::shared_ptr<Actionable_character> Character_factory::create(int id)
{
	switch (id)
	{
	case 1:
		return std::make_shared<Treasure_hunter>();
		break;
	case 2:
		return std::make_shared<Evil_spirit>();
		break;
	default:
		break;
	}
}
