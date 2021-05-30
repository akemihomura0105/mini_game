#include<iostream>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<memory>

template<typename T>
class threadsafe_queue
{
private:

	template<typename T>
	struct Node
	{
		std::shared_ptr<T> data;
		std::unique_ptr<Node<T>> next;
	};

	std::mutex head_mtx, tail_mtx;
	std::unique_ptr<Node<T>> head;
	Node<T>* tail;

	Node<T>* get_tail()
	{
		std::lock_guard<std::mutex>tail_lock(tail_mtx);
		return tail;
	}

	std::unique_ptr<Node<T>> get_head()//lock head_mtx
	{
		std::lock_guard<std::mutex>head_lock(head_mtx);
		if (head.get() == get_tail())
			return nullptr;
		auto result = std::move(head);
		head = std::move(result->next);
		return result;
	}

public:

	std::shared_ptr<T> try_pop()
	{
		auto head_ptr = get_head();
		return (head_ptr ? head_ptr->data : nullptr);
	}

	void push(T data)
	{
		auto new_node = std::make_unique<Node<T>>();
		auto new_data = std::make_shared<T>(move(data));
		auto* new_tail = new_node.get();
		{
			std::lock_guard<std::mutex>tail_lock(tail_mtx);
			tail->data = new_data;
			tail->next = move(new_node);
			tail = new_tail;
		}
	}
	threadsafe_queue() :head(new Node<T>), tail(head.get()) {}
	threadsafe_queue(const threadsafe_queue&) = delete;
	threadsafe_queue& operator=(const threadsafe_queue&) = delete;
};