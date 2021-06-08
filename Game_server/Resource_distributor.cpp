#include "Resource_distributor.h"

Resource_distributor::Resource_distributor(int _size) :
	size(_size),
	new_res(size),
	atk_dag_G(size),
	death_G(size),
	dfn(size),
	low(size),
	vis(size),
	in_deg(size),
	dag_id(size),
	orign_id(size)
{}

void Resource_distributor::solve(const std::vector<std::list<int>>* _atk_init_G, std::vector<std::shared_ptr<Actionable_character>>* _player)
{
	player = _player;
	atk_init_G = _atk_init_G;
	clear();
	get_death_graph();
	tarjan_init();
	get_dag_graph();
	topologic_algorithm();
}

void Resource_distributor::clear()
{
	for (int i = 0; i < size; i++)
	{
		death_G[i].clear();
		dfn[i] = 0;
		low[i] = 0;
		vis[i] = false;
		in_deg[i] = 0;
		id_cnt = 0;
		idx = 0;
		new_res[i].clear();
	}
}

void Resource_distributor::get_death_graph()
{
	for (int n = 0; n < size; n++)
	{
		if ((*player)[n]->isalive())
			continue;
		for (auto to : (*atk_init_G)[n])
		{
			if ((*player)[n]->isalive())
				continue;
			death_G[n].push_back(to);
		}
	}
}

void Resource_distributor::tarjan_init()
{
	for (int n = 0; n < size; n++)
	{
		if (!dfn[n] && !(*player)[n]->isalive())
			tarjan(n, death_G);
	}
}

void Resource_distributor::tarjan(int n, std::vector<std::list<int>> G)
{
	dfn[n] = low[n] = ++idx;
	vis[n] = true;
	st.push(n);
	for (int e : G[n])
	{
		if (!dfn[e])
		{
			tarjan(e, G);
			low[n] = std::min(low[n], low[e]);
		}
		else
			if (vis[e])
				low[n] = std::min(low[n], dfn[e]);
	}
	if (dfn[n] == low[n])
	{
		int t;
		do
		{
			t = st.top();
			st.pop();
			dag_id[t] = id_cnt;
			orign_id[id_cnt] = t;
			vis[t] = false;
		} while (t != n);
		id_cnt++;
	}
}

void Resource_distributor::get_dag_graph()
{
	for (int n = 0; n < size; n++)
	{
		if ((*player)[n]->isalive())
		{
			dag_id[n] = id_cnt++;
			orign_id[dag_id[n]] = n;
		}
	}

	for (int n = 0; n < size; n++)
		new_res[dag_id[n]] += (*player)[n]->get_res();

	for (int n = 0; n < size; n++)
		for (int to : (*atk_init_G)[n])
			if (!(*player)[to]->isalive())
			{
				atk_dag_G[dag_id[to]].push_back(dag_id[n]);
				in_deg[dag_id[n]]++;
			}
}

void Resource_distributor::topologic_algorithm()
{
	std::queue<int>que;
	for (int n = 0; n < atk_dag_G.size(); n++)
		if (!in_deg[n])
			que.push(n);
	while (!que.empty())
	{
		auto n = que.front();
		que.pop();
		if (atk_dag_G[n].empty())
			continue;
		if (!(*player)[orign_id[n]]->isalive())
		{
			auto distribute_vec = new_res[n].divide(atk_dag_G[n].size());
			int cnt = 0;
			for (auto to : atk_dag_G[n])
				new_res[to] += distribute_vec[cnt++];
		}
		for (auto to : atk_dag_G[n])
			if (--in_deg[to] == 0)
				que.push(to);
	}
	for (int n = 0; n < size; n++)
		if (!(*player)[n]->isalive())
			(*player)[n]->clear_res();
	for (int n = 0; n < atk_dag_G.size(); n++)
		if ((*player)[orign_id[n]]->isalive())
			(*player)[orign_id[n]]->set_res(new_res[n]);
}
