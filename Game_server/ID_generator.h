#pragma once
#include<memory>
#include<queue>


template<typename T>
class ID_generator;

template<typename T>
class ID
{
public:
	friend class ID_generator<T>;
	operator T()const { return data; }
	bool isinit()const
	{
		return init;
	}
	void delete_count()
	{
		gen->gc.push(data);
	}
	ID() {}
private:
	T data;
	bool init = false;
	ID_generator<T>* gen;
	ID(ID_generator<T>* _gen) :gen(_gen) {}
};

template<typename T>
class ID_generator
{
	friend class ID<T>;
public:
	std::shared_ptr<ID<T>>generate()
	{
		auto Deleter = [](ID<T>* ptr)
		{
			ptr->delete_count();
			delete ptr;
		};
		auto ptr = std::shared_ptr<ID<T>>(new ID<T>, Deleter);
		ptr->gen = this;
		if (gc.empty())
		{
			if (count == limit)
				throw "generate out of the limit\n";
			ptr->data = count++;
		}
		else
		{
			ptr->data = gc.top();
			gc.pop();
		}
		ptr->init = true;
		return ptr;
	}
	T count;
	T start, limit;
	ID_generator(T _start, T _limit) :start(_start), limit(_limit), count(_start)
	{
	}
private:
	std::priority_queue<T>gc;
};