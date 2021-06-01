#include "Character_factory.h"

std::shared_ptr<Actionable_character> Character_factory::create(int character_id, int game_id, int session_id)
{
	switch (character_id)
	{
	case 1:
		return std::make_shared<Treasure_hunter>(game_id, session_id);
		break;
	case 2:
		return std::make_shared<Evil_spirit>(game_id, session_id);
		break;
	default:
		break;
	}
}
