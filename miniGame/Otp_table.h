#pragma once
#include<vector>
#include<string>
#include<iostream>
#include<numeric>
class Otp_table
{
public:
	void insert(std::vector<std::string>vec);

	friend std::ostream& operator<<(std::ostream& os, const Otp_table& T);

	Otp_table(size_t n);

private:
	std::vector<std::vector<std::string>> table;
	std::vector<size_t>len;
	size_t tot_len = 0;
};

