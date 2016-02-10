#ifndef MTL_FUNCTORS_QUEUE
#define MTL_FUNCTORS_QUEUE
#include<memory>
#include<vector>
#include<map>
#include<functional>
#include<thread>
#include<mutex>

namespace mtl
{

	class functors_queue
	{
	public:
		using Functor = std::function< void(void) >;

		functors_queue();


		void plan_functor(double inSeconds, const Functor& functor);
		void queue_functor(const Functor& functor);

		template<typename T>
		void plan_weak_functor(double inSeconds, const Functor& functor, const std::shared_ptr<T> &object)
		{
			std::weak_ptr<T> weak(object);
			auto wrapper_functor = [=]()
			{
				auto lock = weak.lock();
				if (lock)
					functor();
			};
			planFunctor(inSeconds, wrapper_functor);
		}


		void run();

		bool empty();
	protected:
		double current_time();

		std::multimap<double, Functor> _plannedFunctors;
		std::vector<Functor> _queuedFunctors;
		std::mutex _mutex;
	};


};


#endif
