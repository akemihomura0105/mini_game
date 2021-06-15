#include "state_code.h"



std::string state_code::message_def[1000] = {
	"无消息",
	"登录成功",
	"重复登录",
	"房间满员",
	"房间不存在",
	"你不是房主",
	"房间没有满员",
	"玩家没有全部准备",
	"游戏开始",
	"没有行动力",
	"人物已经死亡",
	"目标与你不是同一地点",
	"攻击成功",
	"没有足够的子弹",
	"没有足够的绷带",
	"目标已死亡",
	"移动成功",
	"不能移动至当前位置",
	"不是你的移动回合",
	"不能移动至基地",
	"技能冷却中",
	"治疗成功",
	"目标生命值已满",
	"挖矿成功",
	"没有足够的金币",
	"出价成功",
	"购买成功",
	"收到探索",
	"探索成功",
	"探索失败"
};


void state_code::set(CODE _code)
{
	code = _code;
}

const std::string& state_code::message()
{
	return message_def[code];
}

state_code::operator int() const
{
	return code;
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

