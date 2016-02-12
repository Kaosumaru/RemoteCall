#ifndef MTL_LOCKLESS_SLIST
#define MTL_LOCKLESS_SLIST

#include <memory>
#include <atomic>

namespace mtl
{
namespace lockless
{

#if 0
template<typename T>
class slist
{
	struct Node { T t; std::shared_ptr<Node> next; };
	std::shared_ptr<Node> head;

public:
	slist() = default;
	~slist() = default;

	class reference {
		std::shared_ptr<Node> _p;
	public:
		reference(const std::shared_ptr<Node> &p) : _p{ p } {}
		T& operator*() { return _p->t; }
		T* operator->() { return &_p->t; }
	};

	auto find(T t) const {
		auto p = std::atomic_load(head);
		while (p && p->t != t)
			p = p->next;
		return reference{ move(p) };
	}

	void push_front(T t) {
		auto p = make_shared<Node>();
		p->t = t;
		p->next = head;
		while (std::atomic_compare_exchange_weak(head, p->next, p))
		{
		}
	}

	auto pop_front() {
		auto p = std::atomic_load(head);
		while (p && !std::atomic_compare_exchange_weak(head, p, p->next))
		{
		}
		return p;
	}
};
#endif

}
}



#endif