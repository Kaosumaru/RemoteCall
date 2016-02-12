#ifndef MTL_FUTURE
#define MTL_FUTURE

#include <memory>
#include <functional>
#include <type_traits>
#include <mutex>

namespace mtl
{
	template<typename T>
	class future;

	namespace impl
	{
		template<typename T>
		struct default_executor
		{
			using callback = std::function<void(T&)>;
			void operator() (const callback& c, T& t)
			{
				c(t);
			}
		};

		template<>
		struct default_executor<void>
		{
			using callback = std::function<void(void)>;
			void operator() (const callback& c)
			{
				c();
			}
		};

		template<typename T, typename Executor = default_executor<T>>
		class promise_base : public std::enable_shared_from_this<promise_base<T>>, protected Executor
		{
		protected:
			template<typename T>
			friend class mtl::future;

			promise_base() {}
			promise_base(const promise_base& other) = delete; // non construction-copyable
			promise_base& operator=(const promise_base&) = delete; // non copyable

		public:

			void set_value(T& t)
			{
				std::lock_guard<std::mutex> guard(_callbacks_mutex);
				_done = true;
				_t = t;
				for (auto &c : _callbacks)
					Executor::operator()(c, _t);
			}

			void set_value(T&& t)
			{
				std::lock_guard<std::mutex> guard(_callbacks_mutex);
				_done = true;
				_t = std::move(t);
				for (auto &c : _callbacks)
					Executor::operator()(c, _t);
			}

		protected:
			using callback = std::function<void(T&)>;

			void add_callback(const callback& c)
			{
				std::lock_guard<std::mutex> guard(_callbacks_mutex);
				if (_done)
				{
					Executor::operator()(c, _t);
					return;
				}
				_callbacks.push_back(c);
			}

			bool       _done = false;
			T          _t;
			std::mutex _callbacks_mutex;
			std::vector<callback> _callbacks;
		};

		template< typename Executor >
		class promise_base<void, Executor> : public std::enable_shared_from_this<promise_base<void>>, protected Executor
		{
		protected:
			template<typename T>
			friend class mtl::future;

			promise_base() {}
			promise_base(const promise_base& other) = delete; // non construction-copyable
			promise_base& operator=(const promise_base&) = delete; // non copyable

		public:
			void set_value()
			{
				std::lock_guard<std::mutex> guard(_callbacks_mutex);
				_done = true;
				for (auto &c : _callbacks)
					Executor::operator()(c);
			}

		protected:
			using callback = std::function<void(void)>;

			void add_callback(const callback& c)
			{
				std::lock_guard<std::mutex> guard(_callbacks_mutex);
				if (_done)
				{
					Executor::operator()(c);
					return;
				}
				_callbacks.push_back(c);
			}

			bool       _done = false;
			std::mutex _callbacks_mutex;
			std::vector<callback> _callbacks;
		};
	}


	template<typename T>
	class future
	{
		using promise = impl::promise_base<T>;
		using promise_pointer = std::shared_ptr<promise>;
	public:
		future(const promise_pointer& pointer) : _base(pointer) {}

		void then(const typename promise::callback& c)
		{
			_base->add_callback(c);
		}
	protected:
		promise_pointer _base;
	};

	template<typename T>
	class promise : public impl::promise_base<T>
	{
	public:
		static std::shared_ptr<promise> create() { return std::make_shared<promise>(); }
		future<T> get_future() { return future<T>(shared_from_this()); }
	};


}



#endif