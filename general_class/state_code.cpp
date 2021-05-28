#include "state_code.h"

const std::string state_code::message_def[1000] = {
	"无消息",
	"登录成功",
	"重复登录",
	"房间满员",
	"房间不存在"
};

void state_code::set(CODE _code)
{
	code = _code;
}

const std::string& state_code::message()
{
	return message_def[code];
}

bool state_code::operator==(const state_code& sc) const
{
	return code == sc.code;
}

bool state_code::operator==(const CODE _code) const
{
	return code == _code;
}

bool state_code::operator!=(const CODE code) const
{
	return !(*this == code);
}

