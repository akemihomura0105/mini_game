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

	void resize(int n);
	Otp_table();
	Otp_table(int n);

private:
	std::vector<std::vector<std::string>> table;
	std::vector<size_t>len;
	int tot_len = 0;
};

