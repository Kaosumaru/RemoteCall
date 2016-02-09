#ifndef MTL_CONTEXT
#define MTL_CONTEXT

#include <memory>

namespace mtl
{
	template <typename T, typename T1 = void>
	class context
	{
	public:
		friend class context_guard;
		class context_guard
		{
		public:
			context_guard() {}
			context_guard(context_guard&) = delete;
			context_guard(context_guard&& other)
			{
				std::swap(_old, other._old);
				std::swap(_hasValue, other._hasValue);
			}

			context_guard(T& _target) : _old(&(context<T, T1>::current())), _hasValue(true) { Reset(_target); }
			~context_guard() { if (_hasValue) context<T, T1>::set_current(*_old); }

			void Reset(T& _target)
			{
				if (!_hasValue)
				{
					_old = &(context<T, T1>::current());
					_hasValue = true;
				}

				context<T, T1>::set_current(_target);
			}
		protected:

			T* _old = nullptr;
			bool _hasValue = false;
		};

		using context_guardPtr = std::unique_ptr<context_guard>;


		static T& current()
		{
			return *_current_static_singleton;
		}

		static bool is_current()
		{
			return _current_static_singleton != 0;
		}



		// create guards
		static auto create_empty_guard()
		{
			return context_guard();
		}

		static context_guard lock(T&& t)
		{
			return context_guard((T&)t);
		}

		static context_guard lock(T& t)
		{
			return context_guard((T&)t);
		}

		static context_guard lock(T* t)
		{
			return context_guard((T&)(*t));
		}

		static context_guard lock(const std::shared_ptr<T>& t)
		{
			return context_guard((T&)*(t.get()));
		}

	protected:

		static void set_current(T& _target)
		{
			_current_static_singleton = &_target;
		}

		thread_local static T* _current_static_singleton;
	};

	template <class T, typename T1> thread_local T* context<T, T1> ::_current_static_singleton = nullptr;

}

#endif