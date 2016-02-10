#include "functors_queue.hpp"

using namespace mtl;

functors_queue::functors_queue()
{
}

void functors_queue::run()
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		std::vector<Functor> movedList;
		std::swap(movedList, _queuedFunctors);


		_mutex.unlock();
		for (auto &f : movedList)
			f();
		_mutex.lock();

		auto seconds = current_time();
		auto pit = _plannedFunctors.begin();
		auto end = _plannedFunctors.end();
		for (; pit != end; pit++)
		{
			if (pit->first > seconds)
				break;
			_mutex.unlock();
			pit->second();
			_mutex.lock();
		}

		if (pit != _plannedFunctors.begin())
			_plannedFunctors.erase(_plannedFunctors.begin(), pit);
	}
}

bool functors_queue::empty()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _queuedFunctors.empty() && _plannedFunctors.empty();
}

void functors_queue::plan_functor(const Duration& time, const Functor& functor)
{
	if (time == time.zero())
	{
		queue_functor(functor);
		return;
	}

	std::lock_guard<std::mutex> lock(_mutex);
	auto time_point = current_time() + time;

	_plannedFunctors.insert(make_pair(time_point, functor));
}

void functors_queue::queue_functor(const Functor& functor)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_queuedFunctors.push_back(functor);
}

