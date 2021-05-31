#include "state_code.h"


std::string state_code::message_def[1000] = {
	"无消息",
	"登录成功",
	"重复登录",
	"房间满员",
	"房间不存在",
	"你不是房主",
	"玩家没有全部准备",
	"没有行动力",
	"没有足够的子弹",
	"没有足够的绷带",
	"目标已死亡",
	"不能移动至当前位置",
	"技能冷却中"
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

