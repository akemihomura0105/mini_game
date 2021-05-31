#pragma once
#include "Character.h"
#include <memory>
class Character_factory
{
public:
	static std::shared_ptr<Actionable_character> create(int id);
};

