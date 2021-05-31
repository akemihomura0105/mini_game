#include "state_code.h"


std::string state_code::message_def[1000] = {
	"����Ϣ",
	"��¼�ɹ�",
	"�ظ���¼",
	"������Ա",
	"���䲻����",
	"�㲻�Ƿ���",
	"���û��ȫ��׼��",
	"û���ж���",
	"û���㹻���ӵ�",
	"û���㹻�ı���",
	"Ŀ��������",
	"�����ƶ�����ǰλ��",
	"������ȴ��"
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

