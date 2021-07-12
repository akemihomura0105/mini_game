#pragma once
#include "../general_class/Resource.h"
#include "Character.h"
#include<vector>
#include<list>
#include<algorithm>
#include<stack>
#include<queue>
#include<memory>
class Resource_distributor
{
public:
	int size = 0;
	Resource_distributor(int _size);
	void solve(const std::vector<std::list<int>>* _atk_init_G, std::vector<std::shared_ptr<Actionable_character>>* player);

private:
	std::vector<std::shared_ptr<Actionable_character>>* player;
	const std::vector<std::list<int>>* atk_init_G;
	std::vector<Resource> new_res;
	std::vector<std::list<int>>atk_dag_G;
	std::vector<std::list<int>>death_G;
	std::vector<int>dfn, low;
	std::vector<bool>vis;
	std::stack<int>st;
	std::vector<int>in_deg;
	int id_cnt = 0;
	std::vector<int>dag_id;
	std::vector<int>orign_id;
	int idx = 0;
	void clear();

	void get_death_graph();

	void tarjan_init();

	void tarjan(int n, std::vector<std::list<int>> G);

	void get_dag_graph();

	void topologic_algorithm();
};