#include "state_code.h"

void state_code::set(CODE _code)
{
	code = _code;
}

bool state_code::operator==(const state_code& sc) const
{
	return code == sc.code;
}

bool state_code::operator==(const CODE _code) const
{
	return code == _code;
}
