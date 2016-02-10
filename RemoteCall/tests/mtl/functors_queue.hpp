#ifndef MTL_FUNCTORS_QUEUE
#define MTL_FUNCTORS_QUEUE
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

namespace mtl
{

	class functors_queue
	{
	public:
		using Duration = std::chrono::duration<double>;
		using Clock = std::chrono::steady_clock;
		using Functor = std::function< void(void) >;
		using TimePoint = std::chrono::time_point<Clock, Duration>;

		functors_queue();


		void plan_functor(const Duration& time, const Functor& functor);
		void queue_functor(const Functor& functor);

		template<typename T>
		void plan_weak_functor(const Duration& time, const Functor& functor, const std::shared_ptr<T> &object)
		{
			std::weak_ptr<T> weak(object);
			auto wrapper_functor = [=]()
			{
				auto lock = weak.lock();
				if (lock)
					functor();
			};
			planFunctor(time, wrapper_functor);
		}


		void run();

		bool empty();
	protected:
		auto current_time()
		{
			return Clock::now();
		}

		std::multimap<TimePoint, Functor> _plannedFunctors;
		std::vector<Functor> _queuedFunctors;
		std::mutex _mutex;
	};

};


#endif
