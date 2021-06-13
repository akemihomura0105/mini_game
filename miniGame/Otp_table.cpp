#include "Otp_table.h"

void Otp_table::insert(std::vector<std::string> vec)
{
	table.push_back(vec);
	for (int i = 0; i < len.size(); i++)
	{
		tot_len -= len[i];
		len[i] = std::max(len[i], vec[i].length());
		tot_len += len[i];
	}
}

void Otp_table::resize(int n)
{
	len.resize(n);
}

Otp_table::Otp_table()
{
}

Otp_table::Otp_table(int n)
{
	len.resize(n);
}

std::ostream& operator<<(std::ostream& os, const Otp_table& T)
{
	if (T.len.size() == 0)
		return os;
	constexpr int blank = 2;
	for (int i = 0; i < T.tot_len + T.len.size() * blank + 1; i++)
		os << '-';
	os << '\n';
	for (int i = 0; i < T.table.size(); i++)
	{
		os << '|';
		for (int j = 0; j < T.table[i].size(); j++)
		{
			os << T.table[i][j];
			for (int k = 0; k < T.len[j] + 1 - T.table[i][j].size(); k++)
				os << ' ';
			os << '|';
		}
		os << '\n';
		for (int i = 0; i < T.tot_len + T.len.size() * blank + 1; i++)
			os << '-';
		os << "\n";
	}
	return os;
}
